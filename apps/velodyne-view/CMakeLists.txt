
# CMake file for the MRPT application:  kinect-3d-view
#  Run with "cmake ." at the root directory
project(velodyne-view)

# ---------------------------------------------
# TARGET:
# ---------------------------------------------
# Define the executable target:
add_executable(${PROJECT_NAME}
               velodyne-view_main.cpp
			    ${MRPT_VERSION_RC_FILE})

# Add the required libraries for linking:


# Dependencies on MRPT libraries:
#  Just mention the top-level dependency, the rest will be detected automatically,
#  and all the needed #include<> dirs added (see the script DeclareAppDependencies.cmake for further details)
DeclareAppDependencies(${PROJECT_NAME} mrpt::hwdrivers mrpt::gui mrpt::maps mrpt::tclap)

DeclareAppForInstall(${PROJECT_NAME})

AppStartMenuLink(${PROJECT_NAME} "Velodyne viewer (VLP-16 mode)")   # Add link to the Start menu (in Windows Installable packages)
