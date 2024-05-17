workspace "Swindow"
   configurations { "Debug", "Release" }
   outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
    startproject "SampleC"

project "Swindow"
   kind "SharedLib"
   language "C"
   targetdir "bin/%{cfg.buildcfg}"

   files 
   { 
        "Swindow/src/**.h", 
        "Swindow/src/**.c" 
    }

   filter "configurations:Debug"
      defines { "SWINDOW_DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "SWINDOW_RELEASE" }
      optimize "On"

project "SampleCSharp"
   kind "ConsoleApp"
   language "C#"
   targetdir "bin/%{cfg.buildcfg}"

   files 
   { 
        "SampleCSharp/src/**.h", 
        "SampleCSharp/src/**.c" 
    }

   filter "configurations:Debug"
      defines { "SWINDOW_DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "SWINDOW_RELEASE" }
      optimize "On"

project "SampleC"
   kind "ConsoleApp"
   language "C"
   targetdir "bin/%{cfg.buildcfg}"

   files 
   { 
        "SampleC/src/**.h", 
        "SampleC/src/**.c" 
    }

    links
    {
        "Swindow"
    }

   filter "configurations:Debug"
      defines { "SWINDOW_DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "SWINDOW_RELEASE" }
      optimize "On"