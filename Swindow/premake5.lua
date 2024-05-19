project "Swindow"
    kind "SharedLib"
    language "C"
    targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
    objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

    files 
    { 
        "src/**.h", 
        "src/**.c" 
    }

    includedirs
    {
        "../external/SDL2/include"
    }

    libdirs
    {
        "../external/SDL2",
    }

    links
    {
        "../external/SDL2/SDL2",
        "../external/SDL2/SDL2main"
    }

    filter "system:windows"
        systemversion "latest"
        defines 
        {
            "SDL_MAIN_HANDLED"  -- Define this if you don't use SDL's main function
        }

    filter "configurations:Debug"
        defines { "SWINDOW_DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "SWINDOW_RELEASE" }
        optimize "On"