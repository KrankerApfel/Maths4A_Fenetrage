/*####################################################################################################################
#																													 #
#																													 #
#																													 #
#										Tom RAKOTOMANAMPISON and Lior DILER                                          #
#													   present                                                       #
#																													 #
#																													 #
#                           __________      .__          _________.__                                                #
#                           \______   \____ |  | ___.__./   _____/|  | _____  ___.__. ___________                    #
#                            |     ___/  _ \|  |<   |  |\_____  \ |  | \__  \<   |  |/ __ \_  __ \                   #
#                            |    |  (  <_> )  |_\___  |/        \|  |__/ __ \\___  \  ___/|  | \/                   #
#                            |____|   \____/|____/ ____/_______  /|____(____  / ____|\___  >__|                      #
#                           					 \/            \/           \/\/         \/                          #
#																													 #
#																													 #
#               					 ~ an OpenGL project to study polygon clipping ~                                 #
#																													 #
#																													 #
#																													 #
#																													 #
#																													 #
#                               	       4th grade 3D and Video Game engineering                                   #
#                                                    ESGI - 2020/2021                                                #																													 #
#																													 #
####################################################################################################################*/



#include "stdafx.h"
#include "src/rendering/Shader.h"
#include "src/rendering/Mesh.h"
#include "src/objects/Polygon.h"
#include "src/objects/Point.h"
#include "src/resources/utils.h"

// ==== gobals vars
enum State { IDLE, MAIN_MENU, DRAW_POLYGON, DRAW_CLIPPING_AREA, COLOR_SELECTION, DISPLAY_RECTBOUNDS } programState;
bool ctrlz = false;
std::array<float, 4> currentColor{ 1, 0, 0, 1 };
std::array<float, 4> clippingColor{ 0, 1, 0, 1 };
std::array<float, 4> rectangleColor{ 0.7, 0.5, 0.12, 1 };
bool leftClick = false;
double mouseX, mouseY;
static int oldState = GLFW_RELEASE;



// ===== utils
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		programState = State::MAIN_MENU;
	}
	int newState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if (newState == GLFW_PRESS && oldState == GLFW_RELEASE)
	{
		leftClick = true;

	}
	oldState = newState;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)) {
		ctrlz = true;
	}
}

Point screenToWorldCoordinateint(double x, double y, GLFWwindow* w) {
	//get screen size
	int Rx, Ry;
	glfwGetWindowSize(w, &Rx, &Ry);

	double worldX = (x / Rx) * 2.0 - 1.0;
	double worldY = 1.0 - (y / Ry) * 2.0;
	return Point(worldX, worldY);
}

static void glfw_error_callback(int error, const char* description)
{
	spdlog::error("Glfw Error {}: {}", error, description);
}

// ==== core
int main()
{
	// Setup window
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;

	// Create window with graphics context
	GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
	if (window == NULL)
		return 1;
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	Shader s("src\\resources\\vert.glsl", "src\\resources\\frag.glsl");

	//	PaintSlayer::Polygon p(s);
		//Shader s2("src\\resources\\vert.glsl", "src\\resources\\frag.glsl");
	PaintSlayer::Polygon clipping_area(s);
	//	Shader s3("src\\resources\\vert.glsl", "src\\resources\\frag.glsl");
		//PaintSlayer::Polygon newPoly(s3);

	std::vector<PaintSlayer::Polygon> polygons;
	std::vector<PaintSlayer::Polygon> clipped_polygons;
	std::vector<PaintSlayer::Polygon> rects;


	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);

	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	ImGui_ImplOpenGL3_Init(glsl_version);


	while (!glfwWindowShouldClose(window)) {

		// === refresh frame
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		}

		// === right click menu
		if (programState == State::MAIN_MENU)
		{
			bool show_main_menu;
			ImGui::Begin("Tool Menu", &show_main_menu);

			if (ImGui::Button("Change color"))
			{
				programState = State::COLOR_SELECTION;

			}
			if (ImGui::Button("Draw polygone"))
			{
				programState = State::DRAW_POLYGON;
				leftClick = false;
				Shader s("src\\resources\\vert.glsl", "src\\resources\\frag.glsl");
				PaintSlayer::Polygon p(s);
				polygons.push_back(p);

			}
			if (ImGui::Button("Draw clipping area"))
			{
				programState = State::DRAW_CLIPPING_AREA;
				leftClick = false;

			}
			if (ImGui::Button("Apply clipping"))
			{
				for (int i = 0; i < polygons.size(); i++)
				{
					vector<Point> newPoints = SutherlandHodgman(polygons[i].getPoints(), clipping_area.getPoints());
					Shader s("src\\resources\\vert.glsl", "src\\resources\\frag.glsl");
					PaintSlayer::Polygon newPoly = PaintSlayer::Polygon(s, newPoints);
					newPoly.setColor(currentColor);
					clipped_polygons.push_back(newPoly);
					polygons[i].clear();
				}
				clipping_area.clear();
			}
			if (ImGui::Button("Draw rect bounds"))
			{

				rects.clear();
				for (int i = 0; i < polygons.size(); i++)
				{
					std::vector<Point> rec = polygons[i].polyRectangle();
					Shader s("src\\resources\\vert.glsl", "src\\resources\\frag.glsl");
					PaintSlayer::Polygon newPoly(s, rec);
					newPoly.setColor(rectangleColor);
					rects.push_back(newPoly);
				}
				for (int i = 0; i < clipped_polygons.size(); i++)
				{
					std::vector<Point> rec = clipped_polygons[i].polyRectangle();
					Shader s("src\\resources\\vert.glsl", "src\\resources\\frag.glsl");
					PaintSlayer::Polygon newPoly(s, rec);
					newPoly.setColor(rectangleColor);
					rects.push_back(newPoly);
				}
			}
			if (ImGui::Button("Clear all"))
			{
				clipping_area.clear();
				for (int i = 0; i < polygons.size(); i++)
				{
					polygons[i].clear();
				}
				for (int i = 0; i < clipped_polygons.size(); i++)
				{
					clipped_polygons[i].clear();
				}
				for (int i = 0; i < rects.size(); i++)
				{
					rects[i].clear();
				}
			}
			ImGui::End();
		}
		// ==== draw polygon
		if (programState == State::DRAW_POLYGON)
		{
			// indication window
			{
				ImGui::Begin("Indications");
				ImGui::Text("add point: left click, ctrl+z: remove point");
				ImGui::End();
			}


			// add points by clicking on screen
			if (leftClick)
			{
				/*if (programState == State::MAIN_MENU)
				{
					continue;
				}*/
				polygons[polygons.size() - 1].setColor(currentColor);
				glfwGetCursorPos(window, &mouseX, &mouseY);
				Point point = screenToWorldCoordinateint(mouseX, mouseY, window);
				polygons[polygons.size() - 1].addPoint(point.getX(), point.getY());
				leftClick = false;
			}
			// remove point by ctrl+z
			if (ctrlz) {
				if (polygons[polygons.size() - 1].getPoints().size() > 0) polygons[polygons.size() - 1].removePoint();
				ctrlz = false;
			}
		}

		// ==== draw clipping area
		if (programState == State::DRAW_CLIPPING_AREA)
		{
			// indication window
			{
				ImGui::Begin("Indications");
				ImGui::Text("add point: left click, ctrl+z: remove point");
				ImGui::End();
			}

			
			// add points by clicking on screen
			if (leftClick)
			{
				//std::array<float, 4> invertColor{ 1 - currentColor.at(0), 1 - currentColor.at(1), 1 - currentColor.at(2), 1 };
				clipping_area.setColor(clippingColor);
				glfwGetCursorPos(window, &mouseX, &mouseY);
				Point point = screenToWorldCoordinateint(mouseX, mouseY, window);
				clipping_area.addPoint(point.getX(), point.getY());
				leftClick = false;
			}
			// remove point by ctrl+z
			if (ctrlz) {
				if (clipping_area.getPoints().size() > 0) clipping_area.removePoint();
				ctrlz = false;
			}
		}

		// ==== change curent color
		if (programState == State::COLOR_SELECTION)
		{
			bool show_color_menu;
			ImGui::Begin("Change Color Menu", &show_color_menu);
			ImGui::ColorEdit4("poly color", (float*)&currentColor);
			ImGui::ColorEdit4("clipping color", (float*)&clippingColor);
			ImGui::ColorEdit4("rectangle color", (float*)&rectangleColor);
			if (show_color_menu == false) programState == State::IDLE;
			ImGui::End();
		}
		processInput(window);


		ImGui::Render();
		glClearColor(0.2f, 0.5f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		//std::array<float, 4> invertColor{ 1 - currentColor.at(0), 1 - currentColor.at(1), 1 - currentColor.at(2), 1 };
	//	clipping_area.setColor(invertColor);
		clipping_area.draw();
		//p.setColor(currentColor);
	//	p.draw();
		//newPoly.draw();
		for (int i = 0; i < polygons.size(); i++)
		{
			polygons[i].setColor(currentColor);
			polygons[i].draw();
		}
		for (int i = 0; i < clipped_polygons.size(); i++)
		{
			//polygons[i].setColor(currentColor);
			clipped_polygons[i].draw();
		}

		for (int i = 0; i < rects.size(); i++)
		{
			//polygons[i].setColor(rectangleColor);
			rects[i].draw();
		}
		glfwPollEvents();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


		glfwSwapBuffers(window);


	}

	// === end
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	for (int i = 0; i < clipped_polygons.size(); i++)
	{
		clipped_polygons[i].terminate();
	}
	for (int i = 0; i < polygons.size(); i++)
	{
		polygons[i].terminate();
	}
	for (int i = 0; i < rects.size(); i++)
	{
		rects[i].terminate();
	}
	clipping_area.terminate();
	return 0;
}