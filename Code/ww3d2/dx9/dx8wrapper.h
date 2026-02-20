/*
**	Command & Conquer Renegade(tm)
**	Copyright 2025 Electronic Arts Inc.
**
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/***********************************************************************************************
 ***              C O N F I D E N T I A L  ---  W E S T W O O D  S T U D I O S               ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : ww3d                                                         *
 *                                                                                             *
 *                     $Archive:: /Commando/Code/ww3d2/dx8wrapper.h                           $*
 *                                                                                             *
 *              Original Author:: Jani Penttinen                                               *
 *                                                                                             *
 *                      $Author:: Patrick                                                     $*
 *                                                                                             *
 *                     $Modtime:: 2/26/02 4:04p                                               $*
 *                                                                                             *
 *                    $Revision:: 90                                                          $*
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef DX8_WRAPPER_H
#define DX8_WRAPPER_H

#include "always.h"
#include "dllist.h"
#include <d3d9.h>
#include "matrix4.h"
#include "statistics.h"
#include "wwstring.h"
#include "lightenvironment.h"
#include "shader.h"
#include "vector4.h"
#include "cpudetect.h"
#include "dx8caps.h"
#include "../ww3dbackend.h"

#include "texture.h"
#include "dx8vertexbuffer.h"
#include "dx8indexbuffer.h"
#include "vertmaterial.h"

/*
** Registry value names
*/
#define	VALUE_NAME_RENDER_DEVICE_NAME					"RenderDeviceName"
#define	VALUE_NAME_RENDER_DEVICE_WIDTH				"RenderDeviceWidth"
#define	VALUE_NAME_RENDER_DEVICE_HEIGHT				"RenderDeviceHeight"
#define	VALUE_NAME_RENDER_DEVICE_DEPTH				"RenderDeviceDepth"
#define	VALUE_NAME_RENDER_DEVICE_WINDOWED			"RenderDeviceWindowed"
#define	VALUE_NAME_RENDER_DEVICE_TEXTURE_DEPTH		"RenderDeviceTextureDepth"

const unsigned MAX_TEXTURE_STAGES=2;

enum {
	BUFFER_TYPE_DX8,
	BUFFER_TYPE_SORTING,
	BUFFER_TYPE_DYNAMIC_DX8,
	BUFFER_TYPE_DYNAMIC_SORTING,
	BUFFER_TYPE_INVALID
};

class VertexMaterialClass;
class CameraClass;
class LightEnvironmentClass;
class RenderDeviceDescClass;
class VertexBufferClass;
class DynamicVBAccessClass;
class IndexBufferClass;
class DynamicIBAccessClass;
class TextureClass;
class LightClass;
class SurfaceClass;
class DX8Caps;

#define DX8_RECORD_MATRIX_CHANGE()				matrix_changes++
#define DX8_RECORD_MATERIAL_CHANGE()			material_changes++
#define DX8_RECORD_VERTEX_BUFFER_CHANGE()		vertex_buffer_changes++
#define DX8_RECORD_INDEX_BUFFER_CHANGE()		index_buffer_changes++
#define DX8_RECORD_LIGHT_CHANGE()				light_changes++
#define DX8_RECORD_TEXTURE_CHANGE()				texture_changes++
#define DX8_RECORD_RENDER_STATE_CHANGE()		render_state_changes++
#define DX8_RECORD_TEXTURE_STAGE_STATE_CHANGE() texture_stage_state_changes++
#define DX8_RECORD_SAMPLER_STATE_CHANGE() 		sampler_state_changes++

extern unsigned number_of_DX8_calls;
extern bool _DX8SingleThreaded;

void DX8_Assert();
void Log_DX8_ErrorCode(HRESULT res);

WWINLINE void DX8_ErrorCode(HRESULT res)
{
	if (res==D3D_OK) return;
	Log_DX8_ErrorCode(res);
}

#ifdef WWDEBUG
#define DX8CALL_HRES(x,res) DX8_Assert(); res = DX8Wrapper::_Get_D3D_Device8()->x; DX8_ErrorCode(res); number_of_DX8_calls++;
#define DX8CALL(x) DX8_Assert(); DX8_ErrorCode(DX8Wrapper::_Get_D3D_Device8()->x); number_of_DX8_calls++;
#define DX8CALL_D3D(x) DX8_Assert(); DX8_ErrorCode(DX8Wrapper::_Get_D3D8()->x); number_of_DX8_calls++;
#define DX8_THREAD_ASSERT() if (_DX8SingleThreaded) { WWASSERT_PRINT(DX8Wrapper::_Get_Main_Thread_ID()==ThreadClass::_Get_Current_Thread_ID(),"DX8Wrapper::DX8 calls must be called from the main thread!"); }
#else
#define DX8CALL_HRES(x,res) res = DX8Wrapper::_Get_D3D_Device8()->x; number_of_DX8_calls++;
#define DX8CALL(x) DX8Wrapper::_Get_D3D_Device8()->x; number_of_DX8_calls++;
#define DX8CALL_D3D(x) DX8Wrapper::_Get_D3D8()->x; number_of_DX8_calls++;
#define DX8_THREAD_ASSERT() ;
#endif

struct RenderStateStruct
{
	ShaderClass shader;
	VertexMaterialClass* material;
	TextureClass * Textures[MAX_TEXTURE_STAGES];
	D3DLIGHT9 Lights[4];
	bool LightEnable[4];
	Matrix4 world;
	Matrix4 view;
	unsigned vertex_buffer_type;
	unsigned index_buffer_type;
	unsigned short vba_offset;
	unsigned short vba_count;
	unsigned short iba_offset;
	VertexBufferClass* vertex_buffer;
	IndexBufferClass* index_buffer;
	unsigned short index_base_offset;

	RenderStateStruct();
	~RenderStateStruct();

	RenderStateStruct& operator= (const RenderStateStruct& src);
};

/**
** DX8Wrapper
**
** DX8 interface wrapper class.  This encapsulates the DX8 interface; adding redundant state
** detection, stat tracking, etc etc.  In general, we will wrap all DX8 calls with at least
** an WWINLINE function so that we can add stat tracking, etc if needed.  Direct access to the
** D3D device will require "friend" status and should be granted only in extreme circumstances :-)
*/
class DX8Wrapper : public WW3DBackend
{
	enum ChangedStates {
		WORLD_CHANGED	=	1<<0,
		VIEW_CHANGED	=	1<<1,
		LIGHT0_CHANGED	=	1<<2,
		LIGHT1_CHANGED	=	1<<3,
		LIGHT2_CHANGED	=	1<<4,
		LIGHT3_CHANGED	=	1<<5,
		TEXTURE0_CHANGED=	1<<6,
		TEXTURE1_CHANGED=	1<<7,
		TEXTURE2_CHANGED=	1<<8,
		TEXTURE3_CHANGED=	1<<9,
		MATERIAL_CHANGED=	1<<14,
		SHADER_CHANGED	=	1<<15,
		VERTEX_BUFFER_CHANGED = 1<<16,
		INDEX_BUFFER_CHANGED = 1 << 17,
		WORLD_IDENTITY=	1<<18,
		VIEW_IDENTITY=		1<<19,

		TEXTURES_CHANGED=
			TEXTURE0_CHANGED|TEXTURE1_CHANGED|TEXTURE2_CHANGED|TEXTURE3_CHANGED,
		LIGHTS_CHANGED=
			LIGHT0_CHANGED|LIGHT1_CHANGED|LIGHT2_CHANGED|LIGHT3_CHANGED,
	};

    void Draw_Sorting_IB_VB(
		unsigned primitive_type,
		unsigned short start_index,
		unsigned short polygon_count,
		unsigned short min_vertex_index,
		unsigned short vertex_count);

    void Draw(
		unsigned primitive_type,
		unsigned short start_index,
		unsigned short polygon_count,
		unsigned short min_vertex_index=0,
		unsigned short vertex_count=0);

public:
    DX8Wrapper();

    bool Init(void * hwnd, bool lite = false);
    void Shutdown(void);

	/*
	** Some WW3D sub-systems need to be initialized after the device is created and shutdown
	** before the device is released.
	*/
    void	 Do_Onetime_Device_Dependent_Inits(void);
    void Do_Onetime_Device_Dependent_Shutdowns(void);

    bool Is_Device_Lost() { return IsDeviceLost; }
    bool Is_Initted(void) { return IsInitted; }

	/*
	** Rendering
	*/
    void Begin_Scene(void);
    void End_Scene(bool flip_frame = true);

	// Flip until the primary buffer is visible.
    void Flip_To_Primary(void);

    void Clear(bool clear_color, bool clear_z_stencil, const Vector3 &color, float z=1.0f, unsigned int stencil=0);

    void	 Set_Viewport(CONST D3DVIEWPORT9* pViewport);

    void Set_Vertex_Buffer(const VertexBufferClass* vb);
    void Set_Vertex_Buffer(const DynamicVBAccessClass& vba);
    void Set_Index_Buffer(const IndexBufferClass* ib,unsigned short index_base_offset);
    void Set_Index_Buffer(const DynamicIBAccessClass& iba,unsigned short index_base_offset);
    void Set_Index_Buffer_Index_Offset(unsigned offset);

    void Get_Render_State(RenderStateStruct& state);
    void Set_Render_State(const RenderStateStruct& state);
    void Release_Render_State();

    void Set_DX8_Material(const D3DMATERIAL9* mat);

    void Set_Gamma(float gamma,float bright,float contrast,bool calibrate=true,bool uselimit=true);

	// Set_ and Get_Transform() functions take the matrix in Westwood convention format.

    void Set_DX8_ZBias(int zbias);
    void 	Set_Pseudo_ZBias(int zbias);
    void Set_Projection_Transform_With_Z_Bias(const Matrix4& matrix,float znear, float zfar);	// pointer to 16 matrices

    void Set_Transform(D3DTRANSFORMSTATETYPE transform,const Matrix4& m);
    void Set_Transform(D3DTRANSFORMSTATETYPE transform,const Matrix3D& m);
    void Get_Transform(D3DTRANSFORMSTATETYPE transform, Matrix4& m);
    void	 Set_World_Identity();
    void Set_View_Identity();
    bool	 Is_World_Identity();
    bool Is_View_Identity();

	// Note that *_DX8_Transform() functions take the matrix in DX8 format - transposed from Westwood convention.

    void _Set_DX8_Transform(D3DTRANSFORMSTATETYPE transform,const Matrix4& m);
    void _Set_DX8_Transform(D3DTRANSFORMSTATETYPE transform,const Matrix3D& m);
    void _Get_DX8_Transform(D3DTRANSFORMSTATETYPE transform, Matrix4& m);

    void Set_DX8_Light(int index,D3DLIGHT9* light);
    void Set_DX8_Render_State(D3DRENDERSTATETYPE state, unsigned value);
    void Set_DX8_Texture_Stage_State(unsigned stage, D3DTEXTURESTAGESTATETYPE state, unsigned value);
    void Set_DX8_N_Patch_Mode(float segments);
    void Set_DX8_Texture_Sampler_State(unsigned sampler, D3DSAMPLERSTATETYPE state, unsigned value);
    void Set_DX8_Texture(unsigned int stage, IDirect3DBaseTexture9* texture);
    void Set_Light_Environment(LightEnvironmentClass* light_env);
    void Set_Fog(bool enable, const Vector3 &color, float start, float end);

    WWINLINE const D3DLIGHT9& Peek_Light(unsigned index);
    WWINLINE bool Is_Light_Enabled(unsigned index);

	// Deferred

    void Set_Shader(const ShaderClass& shader);
    void Get_Shader(ShaderClass& shader);
    void Set_Texture(unsigned stage,TextureClass* texture);
    void Set_Material(const VertexMaterialClass* material);
    void Set_Light(unsigned index,const D3DLIGHT9* light);
    void Set_Light(unsigned index,const LightClass &light);

    void Apply_Render_State_Changes();	// Apply deferred render state changes (will be called automatically by Draw...)

    void Draw_Triangles(
		unsigned buffer_type,
		unsigned short start_index,
		unsigned short polygon_count,
		unsigned short min_vertex_index,
		unsigned short vertex_count);
    void Draw_Triangles(
		unsigned short start_index,
		unsigned short polygon_count,
		unsigned short min_vertex_index,
		unsigned short vertex_count);
    void Draw_Strip(
		unsigned short start_index,
		unsigned short index_count,
		unsigned short min_vertex_index,
		unsigned short vertex_count);

	/*
	** Resources
	*/
    IDirect3DTexture9 * _Create_DX8_Texture(
		unsigned int width,
		unsigned int height,
		WW3DFormat format,
		TextureClass::MipCountType mip_level_count,
		D3DPOOL pool=D3DPOOL_MANAGED,
		bool rendertarget=false);
    IDirect3DTexture9 * _Create_DX8_Texture(const char *filename, TextureClass::MipCountType mip_level_count);
    IDirect3DTexture9 * _Create_DX8_Texture(IDirect3DSurface9 *surface, TextureClass::MipCountType mip_level_count);

    IDirect3DSurface9 * _Create_DX8_Surface(unsigned int width, unsigned int height, D3DPOOL pool, WW3DFormat format);
    IDirect3DSurface9 * _Create_DX8_Surface(const char *filename);
    IDirect3DSurface9 * _Get_DX8_Front_Buffer();
    SurfaceClass * _Get_DX8_Back_Buffer(unsigned int num=0);

    void _Copy_DX8_Rects(
			IDirect3DSurface9* pSourceSurface,
			CONST RECT* pSourceRectsArray,
			UINT cRects,
			IDirect3DSurface9* pDestinationSurface,
			CONST POINT* pDestPointsArray
	);
    void _Read_Texture(
			IDirect3DSurface9* pSourceSurface,
			IDirect3DSurface9* pDestinationSurface
	);

    void _Update_Texture(TextureClass *system, TextureClass *video);
    void Flush_DX8_Resource_Manager();
    unsigned int Get_Free_Texture_RAM();

    unsigned _Get_Main_Thread_ID() { return _MainThreadID; }
    const D3DADAPTER_IDENTIFIER9& Get_Current_Adapter_Identifier() { return CurrentAdapterIdentifier; }

	/*
	** Statistics
	*/
    void Begin_Statistics();
    void End_Statistics();
    unsigned Get_Last_Frame_Matrix_Changes();
    unsigned Get_Last_Frame_Material_Changes();
    unsigned Get_Last_Frame_Vertex_Buffer_Changes();
    unsigned Get_Last_Frame_Index_Buffer_Changes();
    unsigned Get_Last_Frame_Light_Changes();
    unsigned Get_Last_Frame_Texture_Changes();
    unsigned Get_Last_Frame_Render_State_Changes();
    unsigned Get_Last_Frame_Texture_Stage_State_Changes();
    unsigned Get_Last_Frame_DX8_Calls();

    unsigned int Get_FrameCount(void);

	// Needed by shader class
    bool						Get_Fog_Enable() { return FogEnable; }
    D3DCOLOR				Get_Fog_Color() { return FogColor; }

    void _Enable_Triangle_Draw(bool enable) { _EnableTriangleDraw=enable; }
    bool _Is_Triangle_Draw_Enabled() { return _EnableTriangleDraw; }

	/*
	** Additional swap chain interface
	**
	**		Use this interface to render to multiple windows (in windowed mode).
	**	To render to an additional window, the sequence of calls should look
	**	something like this:
	**
	**	DX8Wrapper::Set_Render_Target (swap_chain_ptr);
	**
	**	WW3D::Begin_Render (true, true, Vector3 (0, 0, 0));
	**	WW3D::Render (scene, camera, false, false);
	**	WW3D::End_Render ();
	**
	**	swap_chain_ptr->Present (NULL, NULL, NULL, NULL);
	**
	**	DX8Wrapper::Set_Render_Target ((IDirect3DSurface9 *)NULL);
	**
	*/
    IDirect3DSwapChain9 *	Create_Additional_Swap_Chain (HWND render_window);

	/*
	** Render target interface. If render target format is WW3D_FORMAT_UNKNOWN, current display format is used.
	*/
    TextureClass *	Create_Render_Target (int width, int height, WW3DFormat format);

    void					Set_Render_Target (TextureClass * texture);
    void					Set_Render_Target (IDirect3DSurface9 *render_target, bool use_default_depth_buffer = false);
    void					Set_Render_Target (IDirect3DSwapChain9 *swap_chain);
    bool					Is_Render_To_Texture(void) { return IsRenderToTexture; }

    IDirect3DDevice9* _Get_D3D_Device8() { return D3DDevice; }
    IDirect3D9* _Get_D3D8() { return D3DInterface; }

    const DX8Caps*	Get_Current_Caps() { WWASSERT(CurrentCaps); return CurrentCaps; }

    bool Registry_Save_Render_Device( const char * sub_key );
    bool Registry_Load_Render_Device( const char * sub_key, bool resize_window );

    const char* Get_DX8_Render_State_Name(D3DRENDERSTATETYPE state);
    const char* Get_DX8_Texture_Stage_State_Name(D3DTEXTURESTAGESTATETYPE state);

	// Names of the specific values of render states and texture stage states
    void Get_DX8_Texture_Stage_State_Value_Name(StringClass& name, D3DTEXTURESTAGESTATETYPE state, unsigned value);
    void Get_DX8_Render_State_Value_Name(StringClass& name, D3DRENDERSTATETYPE state, unsigned value);

    const char* Get_DX8_Texture_Sampler_State_Name(D3DSAMPLERSTATETYPE state);
    void Get_DX8_Texture_Sampler_State_Value_Name(StringClass& name, D3DSAMPLERSTATETYPE state, unsigned value);

    const char* Get_DX8_Texture_Address_Name(unsigned value);
    const char* Get_DX8_Texture_Filter_Name(unsigned value);
    const char* Get_DX8_Texture_Arg_Name(unsigned value);
    const char* Get_DX8_Texture_Op_Name(unsigned value);
    const char* Get_DX8_Texture_Transform_Flag_Name(unsigned value);
    const char* Get_DX8_ZBuffer_Type_Name(unsigned value);
    const char* Get_DX8_Fill_Mode_Name(unsigned value);
    const char* Get_DX8_Shade_Mode_Name(unsigned value);
    const char* Get_DX8_Blend_Name(unsigned value);
    const char* Get_DX8_Cull_Mode_Name(unsigned value);
    const char* Get_DX8_Cmp_Func_Name(unsigned value);
    const char* Get_DX8_Fog_Mode_Name(unsigned value);
    const char* Get_DX8_Stencil_Op_Name(unsigned value);
    const char* Get_DX8_Material_Source_Name(unsigned value);
    const char* Get_DX8_Vertex_Blend_Flag_Name(unsigned value);
    const char* Get_DX8_Patch_Edge_Style_Name(unsigned value);
    const char* Get_DX8_Debug_Monitor_Token_Name(unsigned value);
    const char* Get_DX8_Blend_Op_Name(unsigned value);

    void SetRenderType(SceneClass::PolyRenderType type){
        switch(type) {
        case SceneClass::POINT:
            Set_DX8_Render_State(D3DRS_FILLMODE,D3DFILL_POINT);
            break;
        case SceneClass::LINE:
            Set_DX8_Render_State(D3DRS_FILLMODE,D3DFILL_WIREFRAME);
            break;
        case SceneClass::FILL:
            Set_DX8_Render_State(D3DRS_FILLMODE,D3DFILL_SOLID);
            break;
        }
    }

protected:

    bool	Create_Device(void);
    bool Reset_Device(void);
    void Release_Device(void);

    void Reset_Statistics();
    void Enumerate_Devices();
    void Set_Default_Global_Render_States(void);
    void Invalidate_Cached_Render_States(void);

	/*
	** Device Selection Code.
	** For backward compatibility, the public interface for these functions is in the ww3d.
	** header file.  These functions are protected so that we aren't exposing two interfaces.
	*/
    bool Set_Any_Render_Device(void);
    bool	 Set_Render_Device(const char * dev_name,int width=-1,int height=-1,int bits=-1,int windowed=-1,bool resize_window=false);
    bool Set_Render_Device(int dev=-1,int resx=-1,int resy=-1,int bits=-1,int windowed=-1,bool resize_window = false);
    bool Set_Next_Render_Device(void);
    bool Toggle_Windowed(void);

    int	Get_Render_Device_Count(void);
    int	Get_Render_Device(void);
    const RenderDeviceDescClass & Get_Render_Device_Desc(int deviceidx);
    const char * Get_Render_Device_Name(int device_index);
    bool Set_Device_Resolution(int width=-1,int height=-1,int bits=-1,int windowed=-1, bool resize_window=false);
    void Get_Device_Resolution(int & set_w,int & set_h,int & set_bits,bool & set_windowed);
    void Get_Render_Target_Resolution(int & set_w,int & set_h,int & set_bits,bool & set_windowed);
    int	Get_Device_Resolution_Width(void) { return ResolutionWidth; }
    int	Get_Device_Resolution_Height(void) { return ResolutionHeight; }

    bool Registry_Save_Render_Device( const char *sub_key, int device, int width, int height, int depth, bool windowed, int texture_depth);
    bool Registry_Load_Render_Device( const char * sub_key, char *device, int device_len, int &width, int &height, int &depth, int &windowed, int &texture_depth);
    bool Is_Windowed(void) { return IsWindowed; }

    void	 Set_Texture_Bitdepth(int depth)	{ WWASSERT(depth==16 || depth==32); TextureBitDepth = depth; }
    int	Get_Texture_Bitdepth(void)			{ return TextureBitDepth; }

    void	 Set_Swap_Interval(int swap);
    int	Get_Swap_Interval(void);
    void Set_Polygon_Mode(int mode);

	/*
	** Internal functions
	*/
    bool Find_Color_And_Z_Mode(int resx,int resy,int bitdepth,D3DFORMAT * set_colorbuffer,D3DFORMAT * set_zmode);
    bool Find_Color_Mode(D3DFORMAT colorbuffer, int resx, int resy, UINT *mode);
    bool Find_Z_Mode(D3DFORMAT colorbuffer,D3DFORMAT backbuffer, D3DFORMAT *zmode);
    bool Test_Z_Mode(D3DFORMAT colorbuffer,D3DFORMAT backbuffer, D3DFORMAT zmode);
    void Compute_Caps(WW3DFormat display_format);

    void Clear_Full_Screen();

	/*
	** Protected Member Variables
	*/

    RenderStateStruct			render_state;
    unsigned						render_state_changed;

    bool								IsInitted;
    bool								IsDeviceLost;
    void *							Hwnd;
    unsigned						_MainThreadID;

    bool								_EnableTriangleDraw;

    int								CurRenderDevice;
    int								ResolutionWidth;
    int								ResolutionHeight;
    int								BitDepth;
    int								TextureBitDepth;
    bool								IsWindowed;

    D3DMATRIX						old_world;
    D3DMATRIX						old_view;
    D3DMATRIX						old_prj;

    bool								world_identity;
    unsigned						RenderStates[256];
    unsigned						TextureStageStates[MAX_TEXTURE_STAGES][32];
    unsigned						TextureSamplerStates[MAX_TEXTURE_STAGES][14];
    IDirect3DBaseTexture9 *	Textures[MAX_TEXTURE_STAGES];

	// These fog settings are constant for all objects in a given scene,
	// unlike the matching renderstates which vary based on shader settings.
    bool								FogEnable;
    D3DCOLOR						FogColor;

    unsigned						matrix_changes;
    unsigned						material_changes;
    unsigned						vertex_buffer_changes;
    unsigned						index_buffer_changes;
    unsigned						light_changes;
    unsigned						texture_changes;
    unsigned						render_state_changes;
    unsigned						texture_stage_state_changes;
    unsigned						sampler_state_changes;
    bool								CurrentDX8LightEnables[4];

    unsigned int FrameCount;

    DX8Caps*						CurrentCaps;

    D3DADAPTER_IDENTIFIER9		CurrentAdapterIdentifier;

    IDirect3D9 *					D3DInterface;			//d3d8;
    IDirect3DDevice9 *			D3DDevice;				//d3ddevice8;

    IDirect3DSurface9 *			CurrentRenderTarget;
    IDirect3DSurface9 *			DefaultRenderTarget;
    IDirect3DSurface9 *			DefaultDepthBuffer;

    bool								IsRenderToTexture;

    int								ZBias;
    float							ZNear;
    float							ZFar;
    Matrix4							ProjectionMatrix;
    int								BaseVertexIndex;

	friend void DX8_Assert();
	friend class WW3D;
	friend class DX8IndexBufferClass;
	friend class DX8VertexBufferClass;
};


WWINLINE void DX8Wrapper::_Set_DX8_Transform(D3DTRANSFORMSTATETYPE transform,const Matrix4& m)
{
	SNAPSHOT_SAY(("DX8 - SetTransform\n"));
	DX8_RECORD_MATRIX_CHANGE();
	DX8CALL(SetTransform(transform,(D3DMATRIX*)&m));
}


WWINLINE void DX8Wrapper::_Set_DX8_Transform(D3DTRANSFORMSTATETYPE transform,const Matrix3D& m)
{
	SNAPSHOT_SAY(("DX8 - SetTransform\n"));
	DX8_RECORD_MATRIX_CHANGE();
	DX8CALL(SetTransform(transform,(D3DMATRIX*)&m));
}

WWINLINE void DX8Wrapper::_Get_DX8_Transform(D3DTRANSFORMSTATETYPE transform, Matrix4& m)
{
	DX8CALL(GetTransform(transform,(D3DMATRIX*)&m));
}

// ----------------------------------------------------------------------------
//
// Set the index offset for the current index buffer
//
// ----------------------------------------------------------------------------

WWINLINE void DX8Wrapper::Set_Index_Buffer_Index_Offset(unsigned offset)
{
	if (render_state.index_base_offset==offset) return;
	render_state.index_base_offset=offset;
	render_state_changed|=INDEX_BUFFER_CHANGED;
}

// ----------------------------------------------------------------------------
// Set the fog settings. This function should be used, rather than setting the
// appropriate renderstates directly, because the shader sets some of the
// renderstates on a per-mesh / per-pass basis depending on global fog states
// (stored in the wrapper) as well as the shader settings.
// This function should be called rarely - once per scene would be appropriate.
// ----------------------------------------------------------------------------

WWINLINE void DX8Wrapper::Set_Fog(bool enable, const Vector3 &color, float start, float end)
{
	// Set global states
	FogEnable = enable;
	FogColor = Convert_Color(color,0.0f);

	// Invalidate the current shader (since the renderstates set by the shader
	// depend on the global fog settings as well as the actual shader settings)
	ShaderClass::Invalidate();

	// Set renderstates which are not affected by the shader
	Set_DX8_Render_State(D3DRS_FOGSTART, *(DWORD *)(&start));
	Set_DX8_Render_State(D3DRS_FOGEND,   *(DWORD *)(&end));
}

// ----------------------------------------------------------------------------
//
// Set vertex buffer to be used in the subsequent render calls. If there was
// a vertex buffer being used earlier, release the reference to it. Passing
// NULL just will release the vertex buffer.
//
// ----------------------------------------------------------------------------

WWINLINE void DX8Wrapper::Set_DX8_Material(const D3DMATERIAL9* mat)
{
	DX8_RECORD_MATERIAL_CHANGE();
	WWASSERT(mat);
	SNAPSHOT_SAY(("DX8 - SetMaterial\n"));
	DX8CALL(SetMaterial(mat));
}

WWINLINE void DX8Wrapper::Set_DX8_Light(int index, D3DLIGHT9* light)
{
	if (light) {
		DX8_RECORD_LIGHT_CHANGE();
		DX8CALL(SetLight(index,light));
		DX8CALL(LightEnable(index,true));
		CurrentDX8LightEnables[index]=true;
		SNAPSHOT_SAY(("DX8 - SetLight\n"));
	}
	else if (CurrentDX8LightEnables[index]) {
		DX8_RECORD_LIGHT_CHANGE();
		CurrentDX8LightEnables[index]=false;
		DX8CALL(LightEnable(index,false));
		SNAPSHOT_SAY(("DX8 - DisableLight\n"));
	}
}

WWINLINE void DX8Wrapper::Set_DX8_Texture_Stage_State(unsigned stage, D3DTEXTURESTAGESTATETYPE state, unsigned value)
{
	// Can't monitor state changes because setShader call to GERD may change the states!
	if (TextureStageStates[stage][(unsigned int)state]==value) return;
#ifdef MESH_RENDER_SNAPSHOT_ENABLED
	if (WW3D::Is_Snapshot_Activated()) {
		StringClass value_name(0,true);
		Get_DX8_Texture_Stage_State_Value_Name(value_name,state,value);
		SNAPSHOT_SAY(("DX8 - SetTextureStageState(stage: %d, state: %s, value: %s)\n",
			stage,
			Get_DX8_Texture_Stage_State_Name(state),
			value_name));
	}
#endif

	TextureStageStates[stage][(unsigned int)state]=value;
	DX8CALL(SetTextureStageState( stage, state, value ));
	DX8_RECORD_TEXTURE_STAGE_STATE_CHANGE();
}

WWINLINE void DX8Wrapper::Set_DX8_N_Patch_Mode(float segments)
{
	// TODO: proper state tracking!
	DX8CALL(SetNPatchMode(segments));
}

WWINLINE void DX8Wrapper::Set_DX8_Texture_Sampler_State(unsigned sampler, D3DSAMPLERSTATETYPE state, unsigned value)
{
	// Can't monitor state changes because setShader call to GERD may change the states!
	if (TextureSamplerStates[sampler][(unsigned int)state]==value) return;
#ifdef MESH_RENDER_SNAPSHOT_ENABLED
	if (WW3D::Is_Snapshot_Activated()) {
		StringClass value_name(0,true);
		Get_DX8_Texture_Sampler_State_Value_Name(value_name,state,value);
		SNAPSHOT_SAY(("DX8 - SetTextureSamplerState(sampler: %d, state: %s, value: %s)\n",
			sampler,
			Get_DX8_Texture_Sampler_State_Name(state),
			value_name));
	}
#endif

	TextureSamplerStates[sampler][(unsigned int)state]=value;
	DX8CALL(SetSamplerState(sampler, state, value));
    DX8_RECORD_SAMPLER_STATE_CHANGE();
}

WWINLINE void DX8Wrapper::Set_DX8_Texture(unsigned int stage, IDirect3DBaseTexture9* texture)
{
	if (Textures[stage]==texture) return;

	SNAPSHOT_SAY(("DX8 - SetTexture(%x) \n",texture));

	if (Textures[stage]) Textures[stage]->Release();
	Textures[stage] = texture;
	if (Textures[stage]) Textures[stage]->AddRef();
	DX8CALL(SetTexture(stage, texture));
	DX8_RECORD_TEXTURE_CHANGE();
}

WWINLINE void DX8Wrapper::_Copy_DX8_Rects(
  IDirect3DSurface9* pSourceSurface,
  CONST RECT* pSourceRectsArray,
  UINT cRects,
  IDirect3DSurface9* pDestinationSurface,
  CONST POINT* pDestPointsArray
)
{
	if (cRects == 0) {
		DX8CALL(UpdateSurface(pSourceSurface, NULL, pDestinationSurface, NULL));
	} else {
		for (UINT i = 0; i < cRects; i++) {
			DX8CALL(UpdateSurface(pSourceSurface, &pSourceRectsArray[i], pDestinationSurface, &pDestPointsArray[i]));
		}
	}
}

WWINLINE void DX8Wrapper::_Read_Texture(
  IDirect3DSurface9* pSourceSurface,
  IDirect3DSurface9* pDestinationSurface)
{
	DX8CALL(GetRenderTargetData(pSourceSurface, pDestinationSurface));
}

#if defined _MSC_VER && _MSC_VER > 1200 // The ASM function clobber the stack under certain optimisation levels in newer MSVC.
WWINLINE unsigned int DX8Wrapper::Convert_Color(const Vector3& color, const float alpha)
{
	WWASSERT(color.X<=1.0f);
	WWASSERT(color.Y<=1.0f);
	WWASSERT(color.Z<=1.0f);
	WWASSERT(alpha<=1.0f);
	WWASSERT(color.X>=0.0f);
	WWASSERT(color.Y>=0.0f);
	WWASSERT(color.Z>=0.0f);
	WWASSERT(alpha>=0.0f);

	return D3DCOLOR_COLORVALUE(color.X,color.Y,color.Z,alpha);
}
WWINLINE unsigned int DX8Wrapper::Convert_Color(const Vector4& color)
{
	WWASSERT(color.X<=1.0f);
	WWASSERT(color.Y<=1.0f);
	WWASSERT(color.Z<=1.0f);
	WWASSERT(color.W<=1.0f);
	WWASSERT(color.X>=0.0f);
	WWASSERT(color.Y>=0.0f);
	WWASSERT(color.Z>=0.0f);
	WWASSERT(color.W>=0.0f);

	return D3DCOLOR_COLORVALUE(color.X,color.Y,color.Z,color.W);
}

WWINLINE void DX8Wrapper::Clamp_Color(Vector4& color)
{
	for (int i = 0; i < 4; ++i) {
		float f = (color[i] < 0.0f) ? 0.0f : color[i];
		color[i] = (f > 1.0f) ? 1.0f : f;
	}
}
#else

WWINLINE void DX8Wrapper::Get_Render_State(RenderStateStruct& state)
{
	state=render_state;
}

WWINLINE void DX8Wrapper::Get_Shader(ShaderClass& shader)
{
	shader=render_state.shader;
}

WWINLINE void DX8Wrapper::Set_Texture(unsigned stage,TextureClass* texture)
{
	WWASSERT(stage<MAX_TEXTURE_STAGES);
	if (texture==render_state.Textures[stage]) return;
	REF_PTR_SET(render_state.Textures[stage],texture);
	render_state_changed|=(TEXTURE0_CHANGED<<stage);
}

WWINLINE void DX8Wrapper::Set_Material(const VertexMaterialClass* material)
{
	if (material==render_state.material) return;
	REF_PTR_SET(render_state.material,const_cast<VertexMaterialClass*>(material));
	render_state_changed|=MATERIAL_CHANGED;
}

WWINLINE void DX8Wrapper::Set_Shader(const ShaderClass& shader)
{
	if (!ShaderClass::ShaderDirty && ((unsigned&)shader==(unsigned&)render_state.shader)) return;
	render_state.shader=shader;
	render_state_changed|=SHADER_CHANGED;
}

WWINLINE void DX8Wrapper::Set_Projection_Transform_With_Z_Bias(const Matrix4& matrix, float znear, float zfar)
{
	ZFar=zfar;
	ZNear=znear;
	ProjectionMatrix=matrix.Transpose();

	if (!Get_Current_Caps()->Support_ZBias() && ZNear!=ZFar) {
		Matrix4 tmp=ProjectionMatrix;
		float tmp_zbias=ZBias;
		tmp_zbias*=(1.0f/16.0f);
		tmp_zbias*=1.0f / (ZFar - ZNear);
		tmp[2][2]-=tmp_zbias*tmp[3][2];
		DX8CALL(SetTransform(D3DTS_PROJECTION,(D3DMATRIX*)&tmp));
	}
	else {
		DX8CALL(SetTransform(D3DTS_PROJECTION,(D3DMATRIX*)&ProjectionMatrix));
	}
}

WWINLINE void DX8Wrapper::Set_Pseudo_ZBias(int zbias)
{
	if (zbias==ZBias) return;
	if (zbias>15) zbias=15;
	if (zbias<0) zbias=0;
	ZBias=zbias;

	Matrix4 tmp=ProjectionMatrix;
	float tmp_zbias=ZBias;
	tmp_zbias*=(1.0f/64.0f);
	tmp_zbias*=1.0f / (ZFar - ZNear);
	tmp[2][2]-=tmp_zbias*tmp[3][2];
	DX8CALL(SetTransform(D3DTS_PROJECTION,(D3DMATRIX*)&tmp));
}

WWINLINE void DX8Wrapper::Set_Transform(D3DTRANSFORMSTATETYPE transform,const Matrix4& m)
{
	switch ((int)transform) {
	case D3DTS_WORLD:
		render_state.world=m.Transpose();
		render_state_changed|=(unsigned)WORLD_CHANGED;
		render_state_changed&=~(unsigned)WORLD_IDENTITY;
		break;
	case D3DTS_VIEW:
		render_state.view=m.Transpose();
		render_state_changed|=(unsigned)VIEW_CHANGED;
		render_state_changed&=~(unsigned)VIEW_IDENTITY;
		break;
	case D3DTS_PROJECTION:
		{
			Matrix4 ProjectionMatrix=m.Transpose();
			ZFar=0.0f;
			ZNear=0.0f;
			DX8CALL(SetTransform(D3DTS_PROJECTION,(D3DMATRIX*)&ProjectionMatrix));
		}
		break;
	default:
		DX8_RECORD_MATRIX_CHANGE();
		Matrix4 m2=m.Transpose();
		DX8CALL(SetTransform(transform,(D3DMATRIX*)&m2));
		break;
	}
}

WWINLINE void DX8Wrapper::Set_Transform(D3DTRANSFORMSTATETYPE transform,const Matrix3D& m)
{
	Matrix4 m2(m);
	switch ((int)transform) {
	case D3DTS_WORLD:
		render_state.world=m2.Transpose();
		render_state_changed|=(unsigned)WORLD_CHANGED;
		render_state_changed&=~(unsigned)WORLD_IDENTITY;
		break;
	case D3DTS_VIEW:
		render_state.view=m2.Transpose();
		render_state_changed|=(unsigned)VIEW_CHANGED;
		render_state_changed&=~(unsigned)VIEW_IDENTITY;
		break;
	default:
		DX8_RECORD_MATRIX_CHANGE();
		m2=m2.Transpose();
		DX8CALL(SetTransform(transform,(D3DMATRIX*)&m2));
		break;
	}
}

WWINLINE void DX8Wrapper::Set_World_Identity()
{
	if (render_state_changed&(unsigned)WORLD_IDENTITY) return;
	render_state.world.Make_Identity();
	render_state_changed|=(unsigned)WORLD_CHANGED|(unsigned)WORLD_IDENTITY;
}

WWINLINE void DX8Wrapper::Set_View_Identity()
{
	if (render_state_changed&(unsigned)VIEW_IDENTITY) return;
	render_state.view.Make_Identity();
	render_state_changed|=(unsigned)VIEW_CHANGED|(unsigned)VIEW_IDENTITY;
}

WWINLINE bool DX8Wrapper::Is_World_Identity()
{
	return !!(render_state_changed&(unsigned)WORLD_IDENTITY);
}

WWINLINE bool DX8Wrapper::Is_View_Identity()
{
	return !!(render_state_changed&(unsigned)VIEW_IDENTITY);
}

WWINLINE void DX8Wrapper::Get_Transform(D3DTRANSFORMSTATETYPE transform, Matrix4& m)
{
	D3DMATRIX mat;

	switch ((int)transform) {
	case D3DTS_WORLD:
		if (render_state_changed&WORLD_IDENTITY) m.Make_Identity();
		else m=render_state.world.Transpose();
		break;
	case D3DTS_VIEW:
		if (render_state_changed&VIEW_IDENTITY) m.Make_Identity();
		else m=render_state.view.Transpose();
		break;
	default:
		DX8CALL(GetTransform(transform,&mat));
		m=*(Matrix4*)&mat;
		m=m.Transpose();
		break;
	}
}

WWINLINE void DX8Wrapper::Set_Light(unsigned index, const D3DLIGHT9* light)
{
	if (light) {
		render_state.Lights[index]=*light;
		render_state.LightEnable[index]=true;
	}
	else {
		render_state.LightEnable[index]=false;
	}
	render_state_changed|=(LIGHT0_CHANGED<<index);
}

WWINLINE const D3DLIGHT9& DX8Wrapper::Peek_Light(unsigned index)
{
	return render_state.Lights[index];;
}

WWINLINE bool DX8Wrapper::Is_Light_Enabled(unsigned index)
{
	return render_state.LightEnable[index];
}


WWINLINE void DX8Wrapper::Set_Render_State(const RenderStateStruct& state)
{
	if (render_state.index_buffer) {
		render_state.index_buffer->Release_Engine_Ref();
	}

	if (render_state.vertex_buffer) {
		render_state.vertex_buffer->Release_Engine_Ref();
	}

	render_state=state;
	render_state_changed=0xffffffff;

	if (render_state.index_buffer) {
		render_state.index_buffer->Add_Engine_Ref();
	}

	if (render_state.vertex_buffer) {
		render_state.vertex_buffer->Add_Engine_Ref();
	}
}

WWINLINE void DX8Wrapper::Release_Render_State()
{
	if (render_state.index_buffer) {
		render_state.index_buffer->Release_Engine_Ref();
	}

	if (render_state.vertex_buffer) {
		render_state.vertex_buffer->Release_Engine_Ref();
	}

	REF_PTR_RELEASE(render_state.vertex_buffer);
	REF_PTR_RELEASE(render_state.index_buffer);
	REF_PTR_RELEASE(render_state.material);
	for (unsigned i=0;i<MAX_TEXTURE_STAGES;++i) REF_PTR_RELEASE(render_state.Textures[i]);
}


WWINLINE RenderStateStruct::RenderStateStruct()
	:
	material(0),
	vertex_buffer(0),
	index_buffer(0)
{
	for (unsigned i=0;i<MAX_TEXTURE_STAGES;++i) Textures[i]=0;
}

WWINLINE RenderStateStruct::~RenderStateStruct()
{
	REF_PTR_RELEASE(material);
	REF_PTR_RELEASE(vertex_buffer);
	REF_PTR_RELEASE(index_buffer);
	for (unsigned i=0;i<MAX_TEXTURE_STAGES;++i) REF_PTR_RELEASE(Textures[i]);
}


WWINLINE RenderStateStruct& RenderStateStruct::operator= (const RenderStateStruct& src)
{
	REF_PTR_SET(material,src.material);
	REF_PTR_SET(vertex_buffer,src.vertex_buffer);
	REF_PTR_SET(index_buffer,src.index_buffer);
	for (unsigned i=0;i<MAX_TEXTURE_STAGES;++i) REF_PTR_SET(Textures[i],src.Textures[i]);

	LightEnable[0]=src.LightEnable[0];
	LightEnable[1]=src.LightEnable[1];
	LightEnable[2]=src.LightEnable[2];
	LightEnable[3]=src.LightEnable[3];
	if (LightEnable[0]) {
		Lights[0]=src.Lights[0];
		if (LightEnable[1]) {
			Lights[1]=src.Lights[1];
			if (LightEnable[2]) {
				Lights[2]=src.Lights[2];
				if (LightEnable[3]) {
					Lights[3]=src.Lights[3];
				}
			}
		}
	}

	shader=src.shader;
	world=src.world;
	view=src.view;
	vertex_buffer_type=src.vertex_buffer_type;
	index_buffer_type=src.index_buffer_type;
	vba_offset=src.vba_offset;
	vba_count=src.vba_count;
	iba_offset=src.iba_offset;
	index_base_offset=src.index_base_offset;

	return *this;
}


#endif
