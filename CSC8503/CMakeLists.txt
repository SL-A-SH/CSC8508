set(PROJECT_NAME CSC8503)

set(Header_Files
    GameTechRendererInterface.h
    "NetworkedGame.h"
    "NetworkPlayer.h"
    "StateGameObject.h"
    "TutorialGame.h"
)

set(Source_Files
    "Main.cpp"
    "NetworkedGame.cpp"
    "NetworkPlayer.cpp"
    "StateGameObject.cpp"
    "TutorialGame.cpp"
)

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Prospero")
    message("PS5 CSC8503")
    list(APPEND Header_Files GameTechAGCRenderer.h)
    list(APPEND Source_Files GameTechAGCRenderer.cpp)
else()
    message("PC CSC8503")
    list(APPEND Header_Files GameTechRenderer.h)
    list(APPEND Source_Files GameTechRenderer.cpp)
endif()

source_group("Header Files" FILES ${Header_Files})
source_group("Source Files" FILES ${Source_Files})

set(Prison_Escape_Core
    "PrisonEscape/Core/GameBase.cpp"
    "PrisonEscape/Core/GameBase.h"
    "PrisonEscape/Core/GameConfigManager.cpp"
    "PrisonEscape/Core/GameConfigManager.h"
    "PrisonEscape/Core/GameLevelManager.cpp"
    "PrisonEscape/Core/GameLevelManager.h"
    "PrisonEscape/Core/ImGuiManager.cpp"
    "PrisonEscape/Core/ImGuiManager.h"
)
source_group("Prison Escape Core" FILES ${Prison_Escape_Core})

set(Prison_Escape_Levels
    "PrisonEscape/Levels/Level.cpp"
    "PrisonEscape/Levels/Level.h"
    "PrisonEscape/Levels/LevelOne.cpp"
    "PrisonEscape/Levels/LevelOne.h"
    "PrisonEscape/Levels/LevelT.cpp"
    "PrisonEscape/Levels/LevelT.h"
    "PrisonEscape/Levels/SampleLevel.cpp"
    "PrisonEscape/Levels/SampleLevel.h"
)
source_group("Prison Escape Levels" FILES ${Prison_Escape_Levels})

set(Prison_Escape_Prefabs
    "PrisonEscape/Prefabs/PatrolEnemy/PatrollingEnemy.cpp"
    "PrisonEscape/Prefabs/PatrolEnemy/PatrollingEnemy.h"   
    "PrisonEscape/Prefabs/Player/PlayerOne.cpp" 
    "PrisonEscape/Prefabs/Player/PlayerOne.h"
    "PrisonEscape/Prefabs/Player/PlayerTwo.cpp"    
    "PrisonEscape/Prefabs/Player/PlayerTwo.h"
)
source_group("Prison Escape Prefabs" FILES ${Prison_Escape_Prefabs})

set(Prison_Escape_Scripts
    "PrisonEscape/Scripts/PatrolEnemy/PatrolEnemy.cpp"
    "PrisonEscape/Scripts/PatrolEnemy/PatrolEnemy.h"
    "PrisonEscape/Scripts/Player/Player.cpp"
    "PrisonEscape/Scripts/Player/Player.h"
    "PrisonEscape/Scripts/puzzle/Button.cpp"
    "PrisonEscape/Scripts/puzzle/Button.h"
    "PrisonEscape/Scripts/puzzle/puzzleT.cpp"
    "PrisonEscape/Scripts/puzzle/puzzleT.h"
)
source_group("Prison Escape Scripts" FILES ${Prison_Escape_Scripts})

set(Prison_Escape_States
    "PrisonEscape/States/GameplayState.cpp"
    "PrisonEscape/States/GameplayState.h"
    "PrisonEscape/States/GameState.cpp"
    "PrisonEscape/States/GameState.h"
    "PrisonEscape/States/MenuState.cpp"
    "PrisonEscape/States/MenuState.h"

)
source_group("Prison Escape States" FILES ${Prison_Escape_States})

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
    ${Prison_Escape_Core}
    ${Prison_Escape_Levels}
    ${Prison_Escape_Prefabs}
    ${Prison_Escape_Scripts}
    ${Prison_Escape_States}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME}  ${ALL_FILES})
set(ROOT_NAMESPACE CSC8503)

