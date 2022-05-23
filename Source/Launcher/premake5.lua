project "Launcher"
	location "%{wks.location}/Local/"
	kind "WindowedApp"
	language "C++"
	cppdialect "C++17"
	--staticruntime "on"

	targetdir ("../../Bin/")
	targetname("%{prj.name}_%{cfg.buildcfg}")
	objdir ("../../Temp/%{prj.name}/%{cfg.buildcfg}")

	debugdir "../../Bin/"

	files {
		"**.h",
		"**.cpp",
	}

	includedirs {
		"../tga2dcore",
		"../Game/"
	}

	libdirs { "../../Lib/" }

	links { 
		"avcodec.lib",
		"avdevice.lib",
		"avfilter.lib",
		"avformat.lib",
		"avutil.lib",
		"postproc.lib",
		"swresample.lib",
		"swscale.lib",
		"Game",
		"tga2dcore"
	}

	defines {"_CONSOLE"}
	
	filter "configurations:Debug"
		defines {"_DEBUG"}
		runtime "Debug"
		symbols "on"
		links {
			"TGAFBXImporterd.lib"
		}
		
	filter "configurations:Release"
		defines "_RELEASE"
		runtime "Release"
		optimize "on"
		links {
			"TGAFBXImporter.lib"
		}

	filter "configurations:Retail"
		defines "_RETAIL"
		runtime "Release"
		optimize "on"
		links {
			"TGAFBXImporter.lib"
		}

	systemversion "latest"
	
	filter "system:windows"
		symbols "On"		
		systemversion "latest"
		warnings "Extra"
		-- sdlchecks "true"
		--conformanceMode "On"
		--buildoptions { "/STACK:8000000" }
		flags { 
		--	"FatalWarnings", -- would be both compile and lib, the original didn't set lib
			"FatalCompileWarnings",
			"MultiProcessorCompile"
		}
		defines {
			"WIN32",
			"_LIB", 
			"TGE_SYSTEM_WINDOWS"
		}