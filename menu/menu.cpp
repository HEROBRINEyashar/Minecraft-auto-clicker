#include "menu.hpp"

void menu::render_objects( HWND hwnd, int width, int height )
{
	static auto x = 0, y = 0;

	ImGui::SetNextWindowSize( ImVec2( static_cast< float >( width ), static_cast< float >( height ) ), ImGuiCond_Always );
	ImGui::SetNextWindowPos( ImVec2( 0, 0 ), ImGuiCond_Always );

	if ( ImGui::IsMouseClicked( ImGuiMouseButton_Left ) )
		g_menu->get_mouse_offset( x, y, hwnd );

	ImGui::Begin( "##clicker_title", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove );
	{
		if ( y >= 0 && y <= ImGui::GetTextLineHeight( ) + ImGui::GetStyle( ).FramePadding.y * 2.0f && ImGui::IsMouseDragging( ImGuiMouseButton_Left ) )
			g_menu->set_position( x, y, width, height, false, hwnd );

		ImGui::Text( "clicker" );

		ImGui::SameLine( 0.0f, static_cast< float >( width ) - 80.0f );

		ImGui::PushStyleColor( ImGuiCol_Button, color( 255, 255, 255, 0 ) );

		if ( ImGui::Button( ICON_FA_TIMES ) )
		{
			::ShowWindow( hwnd, SW_HIDE );	// Hiding for the string cleaning operation
			std::exit( 0 );					// This will trigger atexit
		}

		ImGui::PopStyleColor( );

		if ( ImGui::BeginTabBar( "##tabs", ImGuiTabBarFlags_None ) )
		{
			if ( ImGui::BeginTabItem( "Clicker" ) )
			{
				ImGui::Text( "Keybindings" );
				ImGui::Separator( );
				{
					ImGui::Combo( "##cmb_kb_type", &config.clicker.activation_type, "Always On\0Hold\0Toggle\0\0" );

					ImGui::SameLine( );

					g_menu->key_bind_button( config.clicker.key, 155, 20 );
					g_menu->activation_type( );
				}

				ImGui::Separator( );
				ImGui::Text( "Clicker configuration" );
				ImGui::Separator( );
				{
					ImGui::Checkbox( "Left clicker enabled##lc_enabled", &config.clicker.left_enabled );

					ImGui::SliderInt( "##l_maxcps", &config.clicker.l_min_cps, 1, 20, "Maximum CPS %d" );
					ImGui::SliderInt( "##l_mincps", &config.clicker.l_max_cps, 1, 20, "Minimum CPS %d" );

					ImGui::Separator( );

					ImGui::Checkbox( "Right clicker enabled##lr_enabled", &config.clicker.right_enabled );

					ImGui::SliderInt( "##r_maxcps", &config.clicker.r_min_cps, 1, 20, "Maximum CPS %d" );
					ImGui::SliderInt( "##r_mincps", &config.clicker.r_max_cps, 1, 20, "Minimum CPS %d" );

					ImGui::Combo( "Client Version##cl_ver", &config.clicker.version_type, "Lunar\0Badlion\0Minecraft / Forge\0Custom\0\0" );

					static char buffer_w[ 16 ];
					switch ( config.clicker.version_type )
					{
						case 0:
							config.clicker.window_title = "Lunar";
							break;
						case 1:
							config.clicker.window_title = "Badlion";
							break;
						case 2:
							config.clicker.window_title = "Minecraft";
							break;
						case 3:
							ImGui::InputText( "Window Title##wnd_title", buffer_w, IM_ARRAYSIZE( buffer_w ) );
							config.clicker.window_title = buffer_w;
							break;
					}

					ImGui::Checkbox( "Blockhit", &config.clicker.blockhit );
					if ( config.clicker.blockhit )
					{
						ImGui::SliderInt( "##blockhit_chance", &config.clicker.blockhit_chance, 1, 100, "Blockhit chance %d%%" );
					}
				}
				ImGui::EndTabItem( );
			}

			if ( ImGui::BeginTabItem( "Config" ) )
			{
				ImGui::Text( "Config settings" );
				ImGui::Separator( );
				{

					if ( ImGui::Button( "Open config folder" ) )
					{
						PIDLIST_ABSOLUTE pidl;
						if ( SUCCEEDED( SHParseDisplayName( util::string_to_wstring( config.config_path.c_str( ) ).c_str( ), 0, &pidl, 0, 0 ) ) )
						{
							ITEMIDLIST idNull = { 0 };
							LPCITEMIDLIST pidlNull[ 1 ] = { &idNull };
							SHOpenFolderAndSelectItems( pidl, 1, pidlNull, 0 );
							ILFree( pidl );
						}
					}

					constexpr auto &config_items = config.get_configs( );
					static int current_config = -1;

					if ( static_cast< size_t >( current_config ) >= config_items.size( ) )
						current_config = -1;

					static char buffer[ 16 ];

					if ( ImGui::ListBox( "Configs", &current_config, [ ]( void *data, int idx, const char **out_text )
						{
							auto &vector = *static_cast< std::vector<std::string> * >( data );
							*out_text = vector[ idx ].c_str( );
							return true;
						}, &config_items, config_items.size( ), 5 ) && current_config != -1 )

						strcpy_s( buffer, config_items[ current_config ].c_str( ) );

						if ( ImGui::InputText( "Config name", buffer, IM_ARRAYSIZE( buffer ), ImGuiInputTextFlags_EnterReturnsTrue ) )
						{
							if ( current_config != -1 )
								config.rename( current_config, buffer );
						}

						if ( ImGui::Button( ( "Create" ), ImVec2( 75, 20 ) ) )
						{
							config.add( buffer );
						}

						ImGui::SameLine( );

						if ( ImGui::Button( ( "Reset" ), ImVec2( 75, 20 ) ) )
						{
							config.reset( );
						}

						if ( current_config != -1 )
						{
							if ( ImGui::Button( ( "Load" ), ImVec2( 75, 20 ) ) )
							{
								config.load( current_config );
							}

							ImGui::SameLine( );

							if ( ImGui::Button( ( "Save" ), ImVec2( 75, 20 ) ) )
							{
								config.save( current_config );
							}

							ImGui::SameLine( );

							if ( ImGui::Button( ( "Delete" ), ImVec2( 75, 20 ) ) )
							{
								config.remove( current_config );
							}
						}
				}

				ImGui::EndTabItem( );
			}

			if ( ImGui::BeginTabItem( "Info" ) )
			{
				ImGui::Text( "Information" );
				ImGui::Separator( );
				{
					ImGui::Text( "Is left button down: %s", vars::b_l_mouse_down ? ICON_FA_CHECK " " : ICON_FA_TIMES " " );
					ImGui::Text( "Is right button down: %s", vars::b_r_mouse_down ? ICON_FA_CHECK " " : ICON_FA_TIMES " " );
					ImGui::Text( "Is hotkey toggled: %s", config.clicker.hotkey_enabled ? ICON_FA_CHECK " " : ICON_FA_TIMES " " );
					ImGui::Text( "Clicks this session: %d", vars::i_clicks_this_session );
					ImGui::Text( "Application average %.1f ms (%.1f fps)", 1000.0f / ImGui::GetIO( ).Framerate, ImGui::GetIO( ).Framerate );
				}
				ImGui::EndTabItem( );
			}

			if ( ImGui::BeginTabItem( "Self-destruct" ) )
			{
				ImGui::Text( "Self-destruct settings" );
				ImGui::Separator( );
				{
					ImGui::Text( "The self-destruct works when you close the program.\nIt will hide itself and exit when the cleaning process finishes.\nYou will hear a beep when it finishes." );

					ImGui::Checkbox( "Delete file on exit", &config.clicker.delete_file_on_exit );

					if ( ImGui::IsItemHovered( ) )
						ImGui::SetTooltip( "Will self delete the executable on exit." );

					ImGui::Checkbox( "Clear strings on exit", &config.clicker.clear_string_on_exit );

					if ( ImGui::IsItemHovered( ) )
						ImGui::SetTooltip( "Will clear strings on explorer matching filename." );

					ImGui::Checkbox( "Clear multibyte strings (slow)", &config.clicker.clear_string_multibyte );

					if ( ImGui::IsItemHovered( ) )
						ImGui::SetTooltip( "Will delete more strings \nat the cost of taking longer to finish the process." );
				}

				ImGui::EndTabItem( );
			}

			ImGui::EndTabBar( );
		}

		if ( config.clicker.l_min_cps <= config.clicker.l_max_cps && !( config.clicker.l_max_cps > 19 ) )
			config.clicker.l_min_cps += 1;

		if ( config.clicker.r_min_cps <= config.clicker.r_max_cps && !( config.clicker.r_max_cps > 19 ) )
			config.clicker.r_min_cps += 1;

		ImGui::End( );
	}
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

LRESULT WINAPI wndproc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	if ( ImGui_ImplWin32_WndProcHandler( hWnd, msg, wParam, lParam ) )
		return true;

	switch ( msg )
	{
		case WM_SIZE:
			if ( g_menu->g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED )
			{
				g_menu->g_d3dpp.BackBufferWidth = LOWORD( lParam );
				g_menu->g_d3dpp.BackBufferHeight = HIWORD( lParam );
				g_menu->reset_device( );
			}
			return 0;
		case WM_SYSCOMMAND:
			if ( ( wParam & 0xfff0 ) == SC_KEYMENU )
				return 0;
			break;
		case WM_DESTROY:
			::PostQuitMessage( 0 );
			return 0;
	}
	return ::DefWindowProc( hWnd, msg, wParam, lParam );
}

bool menu::create( int width, int height )
{
	WNDCLASSEX wc = { sizeof( WNDCLASSEX ), CS_CLASSDC, wndproc, 0L, 0L, GetModuleHandle( NULL ), NULL, NULL, NULL, NULL, _T( "w_w" ), NULL };
	::RegisterClassEx( &wc );
	HWND hwnd = ::CreateWindow( wc.lpszClassName, _T( "" ), WS_POPUP, 100, 100, width, height, NULL, NULL, wc.hInstance, NULL );

	if ( !g_menu->create_device_d3d( hwnd ) )
	{
		g_menu->cleanup_device_d3d( );
		::UnregisterClass( wc.lpszClassName, wc.hInstance );

		return false;
	}

	::ShowWindow( hwnd, SW_SHOWDEFAULT );
	::UpdateWindow( hwnd );

	IMGUI_CHECKVERSION( );
	ImGui::CreateContext( );

	ImGuiIO &io = ImGui::GetIO( ); ( void ) io;
	ImGuiStyle &style = ImGui::GetStyle( ); ( void ) style;

	ImVec4 *colors = style.Colors;

	io.Fonts->AddFontDefault( );
	io.IniFilename = nullptr;

	ImFontConfig f_config;
	f_config.MergeMode = true;
	f_config.PixelSnapH = true;

	static const ImWchar ranges[] =
	{
		ICON_MIN_FA,
		ICON_MAX_FA,
		0
	};

	io.Fonts->AddFontFromMemoryCompressedTTF( fa_compressed_data, fa_compressed_size, 13.0f, &f_config, ranges );

	style.ScrollbarSize = 5.0f;
	style.ChildRounding = 1.0f;
	style.FrameRounding = 3.0f;
	style.GrabRounding = 3.0f;

	// Use demo to see color documentation and stuff
	colors[ ImGuiCol_Text ] = color( 250, 250, 250 );
	colors[ ImGuiCol_TextDisabled ] = color( 204, 204, 204 );
	colors[ ImGuiCol_WindowBg ] = color( 25, 25, 25 );
	colors[ ImGuiCol_PopupBg ] = color( 31, 31, 31 );
	colors[ ImGuiCol_Border ] = color( 244, 154, 255 );
	colors[ ImGuiCol_BorderShadow ] = color( 241, 123, 255 );
	colors[ ImGuiCol_FrameBg ] = color( 32, 32, 32 );
	colors[ ImGuiCol_FrameBgHovered ] = color( 51, 51, 51 );
	colors[ ImGuiCol_FrameBgActive ] = color( 74, 74, 74 );
	colors[ ImGuiCol_Button ] = color( 239, 104, 255 );
	colors[ ImGuiCol_ButtonHovered ] = color( 240, 123, 254 );
	colors[ ImGuiCol_ButtonActive ] = color( 240, 142, 252 );
	colors[ ImGuiCol_ScrollbarGrab ] = color( 244, 154, 255 );
	colors[ ImGuiCol_ScrollbarBg ] = color( 25, 25, 25 );
	colors[ ImGuiCol_ScrollbarGrabHovered ] = color( 240, 123, 254 );
	colors[ ImGuiCol_ScrollbarGrabActive ] = color( 240, 142, 252 );
	colors[ ImGuiCol_SliderGrab ] = color( 239, 104, 255 );
	colors[ ImGuiCol_SliderGrabActive ] = color( 240, 142, 252 );
	colors[ ImGuiCol_CheckMark ] = color( 240, 142, 252 );
	colors[ ImGuiCol_Header ] = color( 240, 142, 252 );
	colors[ ImGuiCol_HeaderHovered ] = color( 240, 123, 254 );
	colors[ ImGuiCol_HeaderActive ] = color( 240, 142, 252 );
	colors[ ImGuiCol_Separator ] = color( 239, 104, 255, 150 );
	colors[ ImGuiCol_Tab ] = color( 239, 104, 255 );
	colors[ ImGuiCol_TabHovered ] = color( 240, 123, 254 );
	colors[ ImGuiCol_TabActive ] = color( 240, 142, 252 );

	ImGui_ImplWin32_Init( hwnd );
	ImGui_ImplDX9_Init( g_pd3dDevice );

	ImVec4 clear_color = color( 0, 0, 0 );

	_log( LDEBUG, "Waiting for program end." );

	MSG msg;
	ZeroMemory( &msg, sizeof( msg ) );
	while ( msg.message != WM_QUIT )
	{
		if ( ::PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
		{
			::TranslateMessage( &msg );
			::DispatchMessage( &msg );
			continue;
		}

		ImGui_ImplDX9_NewFrame( );
		ImGui_ImplWin32_NewFrame( );
		ImGui::NewFrame( );

		// if ( 1000.f / ImGui::GetIO( ).Framerate < 1000.f / 60 )
		//  	std::this_thread::sleep_for( std::chrono::milliseconds( ( long long ) ( 1000.f / 60 ) ) );

		g_menu->render_objects( hwnd, width, height );

		ImGui::EndFrame( );
		g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
		g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		g_pd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );
		D3DCOLOR clear_col_dx = D3DCOLOR_RGBA( ( int ) ( clear_color.x * 255.0f ), ( int ) ( clear_color.y * 255.0f ), ( int ) ( clear_color.z * 255.0f ), ( int ) ( clear_color.w * 255.0f ) );
		g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0 );
		if ( g_pd3dDevice->BeginScene( ) >= 0 )
		{
			ImGui::Render( );
			ImGui_ImplDX9_RenderDrawData( ImGui::GetDrawData( ) );
			g_pd3dDevice->EndScene( );
		}
		HRESULT result = g_pd3dDevice->Present( NULL, NULL, NULL, NULL );

		if ( result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel( ) == D3DERR_DEVICENOTRESET )
			g_menu->reset_device( );
	}

	ImGui_ImplDX9_Shutdown( );
	ImGui_ImplWin32_Shutdown( );
	ImGui::DestroyContext( );

	g_menu->cleanup_device_d3d( );
	::DestroyWindow( hwnd );
	::UnregisterClass( wc.lpszClassName, wc.hInstance );

	return true;
}