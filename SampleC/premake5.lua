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
    }

    links
    {
        "Swindow",
    }

    postbuildcommands
    {
        -- Copy swindow.dll to the target directory
        "{COPYDIR} %{wks.location}/bin/" .. outputdir .. "/Swindow %{cfg.targetdir}"
    }

    filter "system:windows"
    systemversion "latest"
    defines 
    {
        "SW_PLATFORM_WINDOWS" 
    }

    filter "configurations:Debug"
        defines { "SWINDOW_DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "SWINDOW_RELEASE" }
        optimize "On"
