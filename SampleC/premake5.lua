project "SampleC"
    kind "ConsoleApp"
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
        "../Swindow/src",
        "../external/SDL2/include"
    }

    links
    {
        "Swindow",
        "../external/SDL2/SDL2",
        "../external/SDL2/SDL2main"
    }

    libdirs
    {
        "../bin/" .. outputdir .. "/Swindow",
    }

    postbuildcommands
    {
        -- Copy swindow.dll to the target directory
        "{COPYDIR} %{wks.location}/bin/" .. outputdir .. "/Swindow %{cfg.targetdir}"
    }

    filter "configurations:Debug"
        defines { "SWINDOW_DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "SWINDOW_RELEASE" }
        optimize "On"
