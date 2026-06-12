#include "SmearMesh.h"
#include "assimp/anim.h"


SmearMesh::SmearMesh(const std::string &path) : AnimatedMesh(path) {
  generateDeltasSSBO();
}

void SmearMesh::generateDeltasSSBO() {
  // TODO for each bone, calculate all the deltas throughout the animation
  // what data structure to use?
  std::vector<float> frameDeltas;
  std::vector<std::vector<float>> animationDeltas;
  // PSEUDOCODE
  // for (frame in frames) {
  //   calculateFrameDeltas(frame, frameDeltas);
  //   animationDeltas[i] = frameDeltas;
// }
// createAnimationDeltasSSBO(animationDeltas);
}

// TODO add another uniform for the deltas and all the data needed for the smear frames
void SmearMesh::uploadUniforms(const Shader& shader, const RenderContext& ctx) const {
    Mesh::uploadUniforms(shader,ctx);
    std::vector<glm::mat4> transforms;
    getBoneTransforms(transforms);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, bones_data_ssbo);
    for (int i = 0 ; i < m_bonesInfo.size(); ++i) {
        shader.set(("gBones["+std::to_string(i)+"]").c_str(), transforms[i]);
    }
}

void SmearMesh::getFrameDeltas(std::vector<float> &outFrameDeltas) {
  std::vector<glm::mat4> boneTransforms; // global transforms
  getBoneTransforms(boneTransforms);
  outFrameDeltas.assign(m_vertices.size(), 0);

  // iterate through dict entries (pair<key, value>)
  for (std::map<std::string, int>::iterator it = m_boneNameToIndexMap.begin(); it != m_boneNameToIndexMap.end(); ++it) {
    glm::vec3 ct, cr, next_ct, next_cr;
    std::string boneName = it->first;
    int boneIdx = it->second;
    float boneLength = 1.0f; // TODO vai dar pra ser calculado com c_t e c_r

    // how to extract all the transforms from the whole animation?
    // probably go to each animation node (which represents the animation of a
    // single bone throughout the whole animation) and compute the deltas for a
    // single bone.
    // So here I just have to, for each bone, get its vector of transformations
    // in its respective animation node (TODO how to get the animation node
    // associated with it??) and then calculate the deltas based on that
    extractBonePosition(currentBoneTransforms[boneIdx], boneLength, cr, ct);
    extractBonePosition(nextBoneTransforms[boneIdx], boneLength, next_cr, next_ct);

    // and then, how to calculate the final delta for each vertex (that may
    // potentially be affected by many bones)?
    // TODO understand how the fuck they do that in the paper and implement it.
    glm::vec3 vt_hat = glm::normalize(next_ct - ct);
    glm::vec3 vr_hat = glm::normalize(next_cr - cr);
    glm::vec3 b = ct - cr;
    glm::vec3 b_hat = glm::normalize(b);
    float omega = glm::acos(glm::dot(vt_hat, vr_hat));
    auto S = [](const float x) -> float { return glm::smoothstep(0.0f, 1.0f, x); };
    for (int i = 0; i < vertices.size(); ++i) {
      const AnimatedVertex &vertex = vertices[i];
      float weight = 0.0f;
      bool isAttached = false;
      for (int j = 0; j < MAX_NUM_BONES_PER_VERTEX; j++) {
        if (vertex.bonesIDs[j] == boneIdx && vertex.weights[j] > 0.0f) {
          weight = vertex.weights[j];
          isAttached = true;
          break;
        }
      }
      if (!isAttached) continue;

      float u_i =
        S(glm::dot(vertex.position - cr, b_hat) / boneLength);
      glm::vec3 v_hat = sin((1 - u_i) * omega) / sin(omega) * vr_hat + sin(u_i * omega) / sin(omega) * vt_hat;
      glm::vec3 n = v_hat - (glm::dot(v_hat, b_hat)) * b_hat;
      glm::vec3 n_hat = glm::normalize(n);
      float w_coll = 1 - pow(glm::dot(v_hat, b_hat), 2);
      float delta_i = glm::dot(vertex.position - cr, n_hat);
      // TODO account for the influence of multiple bones on this vertex
      // float delta_i_bar = w_coll * delta_i /
    }
  }
}
