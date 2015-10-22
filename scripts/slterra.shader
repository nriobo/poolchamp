// ------------------------------------------
// these shaders are (c) 2002 randy reddig
// ydnar@shaderlab.com   http://shaderlab.com
// they may be used/modified in non-
// commercial projects as long as this notice
// is intact and credit is given
// ------------------------------------------


// this shader is a duplicate of the team arena terrain shader
// shipped with gtkradiant, modified to cast alphashadows

models/mapobjects/tree2/branch2
{
	q3map_lightimage textures/slterra/branch2_light.tga
	
	surfaceparm alphashadow
	surfaceparm pointlight
	surfaceparm trans
	cull none
	sort 10
  	
	{
		clampmap models/mapobjects/tree2/branch2.tga
		blendFunc GL_ZERO GL_ONE_MINUS_SRC_COLOR        
	}
}

// ------------------------------------------
// sky shader, uses a solid background color
// then cloud layer
// then masked off horizon line
// ------------------------------------------

textures/slterra/sky
{
	qer_editorimage textures/slterra/sky_arc_masked.tga
	
	q3map_lightsubdivide 768
	q3map_surfacelight 104
	q3map_backsplash 0 0
	
	q3map_sun 1 .85 0.5 65 -32 56
	
	surfaceparm sky
	surfaceparm noimpact
	surfaceparm nolightmap
	
	skyparms textures/slterra/env/sky 512 -
	
	nopicmip
	nomipmaps
	
	{
		map textures/slterra/sky_clouds.tga
		tcMod scale 3 3
		tcMod scroll 0.005 -0.0125
		rgbGen identityLighting
	}
	{
		clampmap textures/slterra/sky_arc_masked.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		tcMod scale 0.956 0.956	// note for RTCW, this line should be commented out if the sky is knackered
		tcMod transform 1 0 0 1 -1 -1
		rgbGen identityLighting
	}
}

textures/slterra/sky_vx
{
	surfaceparm sky
	surfaceparm noimpact
	surfaceparm nolightmap
	
	skyparms textures/slterra/env/sky 512 -
	
	nopicmip
	nomipmaps
	
	{
		clampmap textures/slterra/sky_arc_masked.tga
		tcMod scale 0.956 0.956	// note for RTCW, this line should be commented out if the sky is knackered
		tcMod transform 1 0 0 1 -1 -1
		rgbGen identityLighting
	}
}

textures/slterra/terrain_0
{
	q3map_lightmapsampleoffset 8.0
	q3map_lightmapaxis z
	q3map_tcGen ivector ( 384 0 0 ) ( 0 384 0 )
	{
		map textures/slterra/rock_1.tga
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		tcGen lightmap
	}
}

textures/slterra/terrain_1
{
	q3map_lightmapsampleoffset 8.0
	q3map_lightmapaxis z
	q3map_tcGen ivector ( 384 0 0 ) ( 0 384 0 )
	{
		map textures/slterra/sand_1.tga
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		tcGen lightmap
	}
}

textures/slterra/terrain_2
{
	q3map_lightmapsampleoffset 8.0
	q3map_lightmapaxis z
	q3map_tcGen ivector ( 384 0 0 ) ( 0 384 0 )
	{
		map textures/slterra/stones_1.tga
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		tcGen lightmap
	}
}

textures/slterra/terrain_0to1
{
	q3map_lightmapsampleoffset 8.0
	q3map_lightmapaxis z
	q3map_tcGen ivector ( 384 0 0 ) ( 0 384 0 )
	
	{
		map textures/slterra/rock_1.tga
	}
	{
		map textures/slterra/sand_1.tga
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		tcGen lightmap
	}
}

textures/slterra/terrain_0to2
{
	q3map_lightmapsampleoffset 8.0
	q3map_lightmapaxis z
	q3map_tcGen ivector ( 384 0 0 ) ( 0 384 0 )
	
	{
		map textures/slterra/rock_1.tga
	}
	{
		map textures/slterra/stones_1.tga
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		tcGen lightmap
	}
}

textures/slterra/terrain_1to2
{
	q3map_lightmapsampleoffset 8.0
	q3map_lightmapaxis z
	q3map_tcGen ivector ( 384 0 0 ) ( 0 384 0 )
	
	{
		map textures/slterra/sand_1.tga
	}
	{
		map textures/slterra/stones_1.tga
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		tcGen lightmap
	}
}

textures/slterra/terrain_vx
{
	{
		map textures/slterra/rock_1.tga
		rgbGen vertex
	}
}
