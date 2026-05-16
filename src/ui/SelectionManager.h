//
// Created by mathi on 16/05/2026.
//

#ifndef STYLIZE_ANIMATION_SELECTIONMANAGER_H
#define STYLIZE_ANIMATION_SELECTIONMANAGER_H

#include <unordered_set>
#include <glm/glm.hpp>
class Mesh;
class Camera;
class Scene;

/**
 * @brief Applies the modifiers on a selection when the users presses keyboard modifiers.
 */
class InputManager {
    bool leftPressed_ = false;
    bool shiftHeld_ = false;

public:
    void onMouseButton(const glm::vec2& mousePos,int button, int action, int mods);
    void onMouseMove(double x, double y);
    void onKey(int key, int action, int mods);
};

/**
 * @brief Interface for choosing an element in the scene.
 */
class IPicker {
public:
    virtual ~IPicker() = default;
    virtual Mesh* pick(glm::vec2 screenPos,
                       const Camera&,
                       const Scene&) = 0;
};

class FrameBufferPicker : public IPicker {
public:
    Mesh* pick(glm::vec2 screenPos,
                       const Camera&,
                       const Scene&) override;
};

/**
 * @brief works with a Picker. Returns the selection of meshes the user selected.
 */
class SelectionManager {
    std::unordered_set<Mesh*> selection;

public:
    void select(Mesh* mesh);
    void deselect(Mesh* mesh);
    const auto& getSelection() const {return selection;}
};



#endif //STYLIZE_ANIMATION_SELECTIONMANAGER_H