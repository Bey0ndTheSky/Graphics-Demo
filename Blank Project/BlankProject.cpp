#include "../nclgl/window.h"
#include "Renderer.h"

int main() {
	Window w("Coursework!", 1280, 720, false);
	if (!w.HasInitialised()) {
		return -1;
	}

	Renderer renderer(w);
	if (!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
		float timestep = w.GetTimer()->GetTimeDeltaSeconds();
		renderer.UpdateScene(timestep);
		renderer.RenderScene();
		renderer.SwapBuffers();
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) {
			Shader::ReloadAllShaders();
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_F1)) {
			renderer.changeScene();
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_F2)) {
			renderer.LockCamera();
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_F3)) {
			renderer.TogglePostProcess();
		}
	}
	return 0;
}