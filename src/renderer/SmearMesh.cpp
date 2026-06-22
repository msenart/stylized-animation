#include <climits>
#include <cstdlib>
#include <string>
#include <fstream>
#include <filesystem>
#include <unistd.h>
#include <random>
#include "SmearMesh.h"
#include "assimp/anim.h"
#include "glm/ext/quaternion_geometric.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/gtx/string_cast.hpp"
#include "renderer/AnimatedMesh.h"


SmearMesh::SmearMesh(const std::string &path) : AnimatedMesh(path) {
  computeVertexSets();
  generateDeltasSSBO(path);

  // DEBUG
  glCreateVertexArrays(1, &m_debugVao);
  glCreateBuffers(1, &m_debugVbo);
  glVertexArrayVertexBuffer(m_debugVao, 0, m_debugVbo, 0, sizeof(glm::vec3));
  glEnableVertexArrayAttrib(m_debugVao, 0);
  glVertexArrayAttribFormat(m_debugVao, 0, 3, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayAttribBinding(m_debugVao, 0, 0);
}

std::string SmearMesh::getDeltasFilePath(const std::string &meshPath) const {
  std::filesystem::path p(meshPath);
  std::string filename = p.stem().string();
  return "assets/deltas/" + filename + ".txt";
}

bool SmearMesh::readFromDeltasFile(const std::string &path,
                                   std::vector<float> &outDeltas,
                                   int &outTotalFrames) const {
  std::ifstream f(path);
  if(!f.is_open()) return false;
  if (!(f >> outTotalFrames)) return false;
  size_t expectedSize = outTotalFrames * m_vertices.size();
  outDeltas.reserve(expectedSize);
  float val;
  while (f >> val) {
    outDeltas.push_back(val);
  }
  f.close();
  if (outDeltas.size() != expectedSize) {
    Log::warn("Cache file " + path + " is corrupted or incomplete. Recalculating...");
    outDeltas.clear();
    return false;
  }
  return true;
}

void SmearMesh::writeToDeltasFile(const std::string &path,
                                  const std::vector<float> &deltas,
                                  int totalFrames) const {
  std::filesystem::create_directory("assets/deltas");
  std::ofstream f(path);
  if (!f.is_open()) {
    Log::warn("Could not open file to write deltas: " + path);
    return;
  }
  f << totalFrames << "\n";
  for (float d : deltas) {
    f << d << "\n";
  }
  f.close();
}


// builds V_kr and V_kt
void SmearMesh::computeVertexSets() {
  m_V_kr.resize(m_bonesInfo.size());
  m_V_kt.resize(m_bonesInfo.size());
  for (auto const& [boneName, boneIdx] : m_boneNameToIndexMap) { // that's cool I didn't know it was possible
    const aiNode *currentNode = m_nodeMap[boneName];
    // tip:
    // 1. bone itself
    // 2. all of its immediate children
    std::vector<int> tipBones = { boneIdx };
    std::vector<const aiNode*> childrenBones;
    getChildrenBones(currentNode, childrenBones);
    for (auto child : childrenBones) {
      tipBones.push_back(m_boneNameToIndexMap[child->mName.C_Str()]);
    }
    // root:
    // 1. bone itself
    // 2. parent
    // 3. siblings
    std::vector<int> rootBones;
    const aiNode* parentBone = getParentBone(currentNode);
    if (parentBone != nullptr) {
      rootBones.push_back(m_boneNameToIndexMap[parentBone->mName.C_Str()]);
      std::vector<const aiNode *> siblingBones;
      getChildrenBones(parentBone, siblingBones);
      for (auto sibling : siblingBones) {
        rootBones.push_back(m_boneNameToIndexMap[sibling->mName.C_Str()]);
      }
    } else {
      rootBones.push_back(boneIdx);
    }
    auto getBonesVertices =
      [&](const std::vector<int> &bones) -> std::vector<int> {
    std::vector<int> vertices;
    for (int i = 0; i < m_vertices.size(); ++i) {
      for (int j = 0; j < MAX_NUM_BONES_PER_VERTEX; ++j) {

        if (std::find(bones.begin(), bones.end(), m_vertices[i].bonesIDs[j]) !=
            bones.end() && m_vertices[i].weights[j] > .0f) {
          vertices.push_back(i);
          break;
        }
      }
    }
    return vertices;
  };
    m_V_kt[boneIdx] = getBonesVertices(tipBones);
    m_V_kr[boneIdx] = getBonesVertices(rootBones);
  }
}

void SmearMesh::generateDeltasSSBO(const std::string& meshPath) {
  std::string deltasPath = getDeltasFilePath(meshPath);
  std::vector<float> flatDeltas;
  int loadedFrames = 0;
  bool loaded = readFromDeltasFile(deltasPath, flatDeltas, loadedFrames);
  if (loaded) {
    m_totalFrames = loadedFrames;
    Log::info("Loaded deltas from : " + deltasPath);
  }

  double ticksPerSecond = m_scene->mAnimations[0]->mTicksPerSecond != 0 ? m_scene->mAnimations[0]->mTicksPerSecond : 25.0f;
  double tickStep = ticksPerSecond / m_Fps;
  double totalDuration = m_scene->mAnimations[0]->mDuration;

  std::vector<glm::mat4> currentTransforms(m_bonesInfo.size());
  std::vector<glm::mat4> nextTransforms(m_bonesInfo.size());
  std::vector<std::vector<float>> animationDeltas;
  std::vector<glm::mat4> flatAnimationBonesTransforms;

  int frameCounter = 0;
  for (double currentTick = 0; currentTick < totalDuration; currentTick += tickStep) {
    double nextTick = currentTick + tickStep;
    if (nextTick > totalDuration) {
      nextTick = fmod(nextTick, totalDuration); // loop over
    }
    calculateGlobalTransformsAtTick(currentTick, m_scene->mRootNode, glm::mat4(1.0f), currentTransforms);
    calculateGlobalTransformsAtTick(nextTick, m_scene->mRootNode, glm::mat4(1.0f), nextTransforms);

    std::vector<glm::mat4> frameSkinnedBones = currentTransforms;
    for (int i = 0; i < m_bonesInfo.size(); ++i) {
      frameSkinnedBones[i] *= m_bonesInfo[i].offsetMatrix;
    }
    flatAnimationBonesTransforms.insert(flatAnimationBonesTransforms.end(), frameSkinnedBones.begin(), frameSkinnedBones.end());

    if (!loaded) {
      std::vector<float> frameDeltas;
      getFrameDeltas(currentTransforms, nextTransforms, frameSkinnedBones, frameDeltas);
      animationDeltas.push_back(frameDeltas);
      flatDeltas.insert(flatDeltas.end(), frameDeltas.begin(), frameDeltas.end());
      Log::info("Deltas generated for frame at tick " + std::to_string(currentTick));
    }
    frameCounter++;
  }
  if (!loaded) {
    m_totalFrames = frameCounter;
    writeToDeltasFile(deltasPath, flatDeltas, m_totalFrames);
    Log::info("Saved deltas to: " + deltasPath);
  }
  flatDeltas.reserve(
      m_totalFrames *
      m_vertices
          .size()); // reserve() makes element insertion easier with insert()
  for (const auto &frameDeltas : animationDeltas) {
    flatDeltas.insert(flatDeltas.end(), frameDeltas.begin(), frameDeltas.end());
  }
  glCreateBuffers(1, &m_deltas_ssbo);
  glNamedBufferData(m_deltas_ssbo, flatDeltas.size() * sizeof(float),
                    flatDeltas.data(), GL_STATIC_DRAW);

  // NOTE maybe either change the function's name or do that somewhere else
  // because it can be confusing later to find where I create the transforms
  // SSBO
  glCreateBuffers(1, &m_animation_bones_transforms_ssbo);
  glNamedBufferData(m_animation_bones_transforms_ssbo,
                    flatAnimationBonesTransforms.size() * sizeof(glm::mat4),
                    flatAnimationBonesTransforms.data(), GL_STATIC_DRAW);
}

// TODO add another uniform (SSBO) for the deltas and all the data needed for the smear frames;
void SmearMesh::uploadUniforms(const Shader& shader, const RenderContext& ctx) const {
    AnimatedMesh::uploadUniforms(shader,ctx);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_deltas_ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, m_animation_bones_transforms_ssbo);

    double time = timer.getCurrentValue();
    double ticks_per_second = m_scene->mAnimations[0]->mTicksPerSecond != 0 ? m_scene->mAnimations[0]->mTicksPerSecond : 25.0f;
    double currentTick = fmod(time * ticks_per_second, m_scene->mAnimations[0]->mDuration);
    double tickStep = ticks_per_second / m_Fps;
    int currentFrame = static_cast<int>(currentTick / tickStep) % m_totalFrames;

    shader.set("currentFrame", currentFrame);
    shader.set("totalVertices", static_cast<int>(m_vertices.size()));
    shader.set("totalFrames", m_totalFrames);
    shader.set("betaMax", 1.0f);
    shader.set("boneStride", static_cast<int>(m_bonesInfo.size())); // in the shader the max number of bones is hardcoded

    std::vector<glm::mat4> currentTransforms;
    getBoneTransforms(currentTransforms);

    std::string boneName = "mixamorig:RightForeArm";
    auto it = m_nodeMap.find("mixamorig:RightForeArm");
    if (it != m_nodeMap.end()) {
      const aiNode* debugNode = it->second;
      glm::vec3 c_r, c_t;
      const_cast<SmearMesh*>(this)->getBonePosition(debugNode, currentTransforms, c_r, c_t);
      shader.set("debug_c_r", c_r);
      shader.set("debug_c_t", c_t);
    }
}

// calculate the deltas for a single frame
void SmearMesh::getFrameDeltas(std::vector<glm::mat4> currentTransforms,
                               std::vector<glm::mat4> nextTransforms,
                               std::vector<glm::mat4> frameSkinnedBones,
                               std::vector<float> &outFrameDeltas
) {
  // NOTE
  // bone variables:
  // - v_t, v_r, c_r, c_t, b, omega
  // vertex variables:
  // - u_i, v, n
  outFrameDeltas.assign(m_vertices.size(), 0);
  // NOTE structure it in a way that when I access u_k, I have a list
  //      of its values for all the bones that affect it. same for d_ik
  std::vector<std::vector<float>> us; // u_ik
  std::vector<std::vector<float>> deltas; // d_ik

  // for all bones (nodes in the tree)
  for (auto const& [boneName, boneIdx] : m_boneNameToIndexMap) {
    glm::vec3 ct, cr, next_ct, next_cr;
    const aiNode* currentNode = m_nodeMap[boneName];

    // get current and next bone positions
    getBonePosition(currentNode, currentTransforms, cr, ct);
    getBonePosition(currentNode, nextTransforms, next_cr, next_ct);

    // calculate bone-dependent variables
    glm::vec3 vr = next_cr - cr;
    glm::vec3 vt = next_ct - ct;
    float vtLen = glm::length(vt);
    float vrLen = glm::length(vr);
    if (vrLen < 1e-8f && vtLen < 1e-8f) continue;
    // TODO maybe not the best fallback
    // consider that tip is moving like the root or vice-versa as fallback
    glm::vec3 vr_norm = glm::normalize(vr);
    glm::vec3 vt_norm = glm::normalize(vt);
    glm::vec3 b = ct - cr;
    if (glm::length(b) < 1e-8f) continue;
    glm::vec3 b_norm = glm::normalize(b);
    float omega = glm::acos(glm::dot(vt_norm, vr_norm));
    auto S = [](const float x) -> float {
      float y = glm::clamp(x, 0.0f, 1.0f);
      return 3 * pow(y, 2) - 2 * pow(y, 3);
    };

    auto calculateDelta_ik = [&](int vertexIdx) -> float {
      const AnimatedVertex& v = m_vertices[vertexIdx];
      // NOTE not sure if I have to put it in skinning (bone) space
      glm::mat4 skinMatrix = glm::mat4(0.0f);
      for (int j = 0; j < MAX_NUM_BONES_PER_VERTEX; ++j) {
        if (v.weights[j] > 0.0f) {
          glm::mat4 boneTransform = currentTransforms[v.bonesIDs[j]] * m_bonesInfo[v.bonesIDs[j]].offsetMatrix;
          skinMatrix += boneTransform * v.weights[j];
        }
      }
      glm::vec3 p_i = glm::vec3(skinMatrix * glm::vec4(v.position, 1.0f));
      // glm::vec3 p_i = m_vertices[vertexIdx].position;
      float u =
        S(glm::dot(p_i - cr, b_norm) / glm::length(b));
      glm::vec3 v_norm;
      if (omega < 0.1f) {
        v_norm = glm::normalize(glm::mix(vr_norm, vt_norm, u));

      } else {
        v_norm = glm::sin((1 - u) * omega) / glm::sin(omega) * vr_norm
          + glm::sin(u * omega) / glm::sin(omega) * vt_norm;
      }
      glm::vec3 n = v_norm - (glm::dot(v_norm, b_norm)) * b_norm;
      return glm::dot(p_i - cr, glm::normalize(n));
    };

    // calculate max delta for normalization
    float maxRootDelta = 0.05f;
    for (int vertexIdx : m_V_kr[boneIdx]) {
      float delta = calculateDelta_ik(vertexIdx);
      if (glm::abs(delta) > maxRootDelta) maxRootDelta = delta;
    }
    float maxTipDelta = 0.05f;
    for (int vertexIdx : m_V_kt[boneIdx]) {
      float delta = calculateDelta_ik(vertexIdx);
      if (glm::abs(delta) > maxTipDelta) maxTipDelta = delta;
    }
    // TEST calculate max delta for a whole bone
    float maxDelta = 0.05f;
    for (int i = 0; i < m_vertices.size(); ++i) {
      for (int j = 0; j < MAX_NUM_BONES_PER_VERTEX; ++j) {
        if (m_vertices[i].bonesIDs[j] == boneIdx && m_vertices[i].weights[j] > 0.0f) {
          float delta = calculateDelta_ik(i);
          if (glm::abs(delta) > maxDelta) maxDelta = delta;
        }
      }
    }

    for (int i = 0; i < m_vertices.size(); ++i) {
      // get the vertices that are affected by this bone
      float weight = 0.0f;
      for (int j = 0; j < MAX_NUM_BONES_PER_VERTEX; ++j) {
        if (m_vertices[i].bonesIDs[j] == boneIdx && m_vertices[i].weights[j] > 0.0f) {
          // int originalBoneIdx = m_vertices[i].bonesIDs[j];
          // int effectiveBoneIdx = getPrunedBoneIdx(m_vertices[i].bonesIDs[j]);
          // if (effectiveBoneIdx == originalBoneIdx) {
          //   weight += m_vertices[i].weights[j];
          //   break;
          // }
          weight += m_vertices[i].weights[j];
          break;
        }
      }
      if (weight == 0) continue;

      // TODO solve this duplication
      const AnimatedVertex& vertex = m_vertices[i];
      glm::mat4 skinMatrix = glm::mat4(0.0f);
      for (int j = 0; j < MAX_NUM_BONES_PER_VERTEX; ++j) {
        if (vertex.weights[j] > 0.0f) {
          glm::mat4 boneTransform = currentTransforms[vertex.bonesIDs[j]] * m_bonesInfo[vertex.bonesIDs[j]].offsetMatrix;
          skinMatrix += boneTransform * vertex.weights[j];
        }
      }
      glm::vec3 p_i = glm::vec3(skinMatrix * glm::vec4(vertex.position, 1.0f));
      float u_ik = S(glm::dot(p_i - cr, b_norm) / glm::length(b));
      glm::vec3 v_norm;
      if (omega < 0.1f) {
          v_norm = glm::normalize(glm::mix(vr_norm, vt_norm, u_ik));
      } else {
          v_norm = glm::sin((1.0f - u_ik) * omega) / glm::sin(omega) * vr_norm
                 + glm::sin(u_ik * omega) / glm::sin(omega) * vt_norm;
      }
      glm::vec3 n = v_norm - (glm::dot(v_norm, b_norm)) * b_norm;
      glm::vec3 n_norm = glm::normalize(n);
      float w_coll = 1.0f - pow(glm::dot(v_norm, b_norm), 2);
      float delta_ik = glm::dot(p_i - cr, n_norm);
      float M_ik = (u_ik * glm::abs(maxTipDelta)) + ((1.0f - u_ik) * glm::abs(maxRootDelta));
      // TEST use per-bone max delta
      M_ik = (u_ik * glm::abs(maxDelta)) + ((1.0f - u_ik) * glm::abs(maxDelta));
      float delta_ik_norm = 0;
      if (M_ik > 0.05) {
        delta_ik_norm = w_coll * (delta_ik / M_ik);
      }
      // delta_ik_norm = delta_ik;
      outFrameDeltas[i] += weight * delta_ik_norm;
    }
  }
}

void SmearMesh::drawDebugBones(const Shader& shader, const RenderContext& ctx) const {
    std::vector<glm::mat4> currentTransforms;
    getBoneTransforms(currentTransforms);

    std::vector<glm::vec3> lineVertices;
    lineVertices.reserve(m_boneNameToIndexMap.size() * 2);

    for (const auto& pair : m_boneNameToIndexMap) {
        const aiNode* node = m_nodeMap.at(pair.first);
        glm::vec3 cr, ct;

        const_cast<SmearMesh*>(this)->getBonePosition(node, currentTransforms, cr, ct);

        lineVertices.push_back(cr);
        lineVertices.push_back(ct);
    }

    // Upload to GPU and draw
    glNamedBufferData(m_debugVbo, lineVertices.size() * sizeof(glm::vec3), lineVertices.data(), GL_DYNAMIC_DRAW);

    glBindVertexArray(m_debugVao);
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(lineVertices.size()));
    glBindVertexArray(0);
}

int SmearMesh::getCurrentFrame() {
  double time = timer.getCurrentValue();
  double ticks_per_second = m_scene->mAnimations[0]->mTicksPerSecond != 0 ? m_scene->mAnimations[0]->mTicksPerSecond : 25.0f;
  double currentTick = fmod(time * ticks_per_second, m_scene->mAnimations[0]->mDuration);
  double tickStep = ticks_per_second / m_Fps;
  return static_cast<int>(currentTick / tickStep) % m_totalFrames;
}

// given a bone index, return its closest ancestor which is not pruned
int SmearMesh::getPrunedBoneIdx(int boneIdx) {
    std::string boneName = "";
    for (const auto& pair : m_boneNameToIndexMap) {
        if (pair.second == boneIdx) {
            boneName = pair.first;
            break;
        }
    }
    bool isPruned = false;
    for (const std::string& pruned : m_pruneBoneRoots) {
        if (boneName.find(pruned) != std::string::npos) {
            isPruned = true;
            break;
        }
    }
    if (!isPruned) {
        return boneIdx;
    }
    const aiNode* currentNode = m_nodeMap[boneName];
    const aiNode* parentNode = getParentBone(currentNode);
    if (parentNode) {
        int parentIdx = m_boneNameToIndexMap[parentNode->mName.C_Str()];
        return getPrunedBoneIdx(parentIdx);
    }
    return boneIdx;
}
// helpers
