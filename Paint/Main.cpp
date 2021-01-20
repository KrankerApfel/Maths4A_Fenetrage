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
enum State { IDLE, MAIN_MENU, DRAW_POLYGON, DRAW_CLIPPING_AREA, COLOR_SELECTION } programState;
bool ctrlz = false;
std::array<float, 4> currentColor{ 1, 0, 0, 1 };
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
	if ( newState == GLFW_PRESS && oldState == GLFW_RELEASE)
	{
		leftClick = true;
	
	}
	oldState = newState;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS &&(glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)) {
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
	Shader s2("src\\resources\\vert.glsl", "src\\resources\\frag.glsl");
	PaintSlayer::Polygon p(s);
	PaintSlayer::Polygon clipping_area(s2);
	Shader s3("src\\resources\\vert.glsl", "src\\resources\\frag.glsl");
	PaintSlayer::Polygon newPoly(s3);


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

			}
			if (ImGui::Button("Draw clipping area"))
			{
				programState = State::DRAW_CLIPPING_AREA;
			}
			if (ImGui::Button("Apply clipping"))
			{
				vector<Point> newPoints = SutherlandHodgman(p.getPoints(), clipping_area.getPoints(), 0);
				newPoly = PaintSlayer::Polygon(s3, newPoints);
				newPoly.setColor(currentColor);
				p.clear();
				clipping_area.clear();
			}
			if (ImGui::Button("Fill"))
			{
				vector<Point> points{ Point(0,0), Point(0,10), Point(3,5), Point(5,5), Point(6,6) };
				
				bool isInside = p.isInside(Point(3, 6), points);
				cout << "isInside: " << isInside;
				/*
				vector<Point> rect = p.polyRectangle(p.getPoints());
				cout << "isInside: " << isInside;
				cout << "rect is: ";
				for (int i = 0; i < rect.size(); i++)
				{
					cout << rect[i].getX() <<  " - " << rect[i].getY() << "\n";
				}*/
				//fillPolygon(p)


				Point pt(3, 4);
				std::vector<Point> points{ Point(0,0), Point(0,10), Point(10,10), Point(0,10) };

				bool a = isPointInsidePoligon(pt, points );
				spdlog::info("inside ? {} ", a);

			}
			if (ImGui::Button("Clear all"))
			{
				p.clear();
				clipping_area.clear();
				newPoly.clear();
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
				p.setColor(currentColor);
				glfwGetCursorPos(window, &mouseX, &mouseY);
				Point point = screenToWorldCoordinateint(mouseX, mouseY, window);
				p.addPoint(point.getX(), point.getY());
				leftClick = false;
			}
			// remove point by ctrl+z
			if (ctrlz) {
				if(p.getPoints().size() > 0) p.removePoint();
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
				std::array<float, 4> invertColor{ 1- currentColor.at(0), 1- currentColor.at(1), 1- currentColor.at(2), 1 };
				clipping_area.setColor(invertColor);
				glfwGetCursorPos(window, &mouseX, &mouseY);
				Point point = screenToWorldCoordinateint(mouseX, mouseY, window);
				clipping_area.addPoint(point.getX(), point.getY());
				leftClick = false;
			}
			// remove point by ctrl+z
			if (ctrlz) {
				if(clipping_area.getPoints().size() > 0) clipping_area.removePoint();
				ctrlz = false;
			}
		}

		// ==== change curent color
		if (programState == State::COLOR_SELECTION)
		{
			bool show_color_menu;
			ImGui::Begin("Change Color Menu", &show_color_menu);
			ImGui::ColorEdit4("color", (float*)&currentColor);
			if (show_color_menu == false) programState == State::IDLE;
			ImGui::End();
		}
		processInput(window);


		ImGui::Render();
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		clipping_area.draw();
		p.draw();
		newPoly.draw();

		glfwPollEvents();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


		glfwSwapBuffers(window);


	}

	// === end
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	p.terminate();
	clipping_area.terminate();
	return 0;
}