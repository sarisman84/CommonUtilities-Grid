project "External"
	location "%{wks.location}/Local/"
		
	language "C++"
	cppdialect "C++17"

	targetdir ("../../Lib/")
	targetname("%{prj.name}_%{cfg.buildcfg}")
	objdir ("../../Temp/%{prj.name}/%{cfg.buildcfg}")

	--pchheader "stdafx.h"
	--pchsource "Source/%{prj.name}/stdafx.cpp"

	files {
		"**.h",
		"**.hpp",
		"**.inl",
		"**.c",
		"**.cpp",
	}

	excludes {
		"ffmpeg-2.0/**.h",
		"ffmpeg-2.0/**.c",
		"ffmpeg-2.0/**.cpp",
		"DirectXTex/DirectXTex/**",
		"DirectXTex/DDSView/**",
		"DirectXTex/Texassemble/**",
		"DirectXTex/Texconv/**",
		"DirectXTex/Texdiag/**",
		"DirectXTex/DDSTextureLoader/DDSTextureLoader9.**",
		"DirectXTex/DDSTextureLoader/DDSTextureLoader12.**",
		"DirectXTex/ScreenGrab/ScreenGrab9.**",
		"DirectXTex/ScreenGrab/ScreenGrab12.**",
		"DirectXTex/WICTextureLoader/WICTextureLoader9.**",
		"DirectXTex/WICTextureLoader/WICTextureLoader12.**",
	}

	includedirs {
		".",
		"../tga2dcore/",
		"source/",
		"DirectXTex/",
		"ffmpeg-2.0/",
		"imgui/",
		--?? on these, spine part of assimp?
		"spine/include/",
		"vld"
	}

	libdirs { "Lib/" }

	defines {"_CONSOLE"}

	filter "configurations:Debug"
		defines {"_DEBUG"}
		runtime "Debug"
		symbols "on"
	filter "configurations:Release"
		defines "_RELEASE"
		runtime "Release"
		optimize "on"
	filter "configurations:Retail"
		defines "_RETAIL"
		runtime "Release"
		optimize "on"

	filter "system:windows"
		kind "StaticLib"
		staticruntime "off"
		symbols "On"		
		systemversion "latest"
		--warnings "Extra"
		--conformanceMode "On"
		--buildoptions { "/permissive" }
		flags { 
		--	"FatalWarnings", -- would be both compile and lib, the original didn't set lib
		--	"FatalCompileWarnings",
			"MultiProcessorCompile"
		}
		links {
			"DXGI",
			"dxguid",
		}

		defines { "_WIN32_WINNT=0x0601" }