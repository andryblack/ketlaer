
#include "config.h"
#include "vbox.hpp"
#include "vbox_config.hpp"
#include "gettext.hpp"
#include "graphics.hpp"
#include "busy_indicator.hpp"

#include "startmenu.hpp"
#include "updater.hpp"
// #include "common-feature.hpp"
#include <string>
#include <sstream>
#include "simplefile.hpp"

#ifdef use_notify_area
#include "notify.hpp"
#include "boost.hpp"
#endif

#include "feature/audio/audio_s.hpp"

#ifdef use_nls
#include <langinfo.h>
#include <locale.h>
#endif
using namespace std;

VBox::VBox()
{
#ifdef use_nls
    setlocale(LC_ALL, ""); /* set from the environment variables */
    bindtextdomain("mms-vbox", mms_prefix"/share/locale");
    bind_textdomain_codeset("mms-vbox", nl_langinfo(CODESET));
#endif
    curlayer = 0;
#ifdef use_notify_area
    nArea->add(NotifyElement("Vbox", boost::bind(&VBox::ShowNotifyArea, this))); 
#endif
    
    as = S_Audio_s::get_instance();
    // as->set_audio(this);

    vbox_conf = S_VBoxConfig::get_instance();
    
    num_vbox = 0;
    vboxes = 0;
    intervall = INTERVALL + 1;
    
    vbox_configs = vbox_conf->p_vbox(); 	// make compiler happy

    foreach (string& vb, vbox_configs) {
    	
    	size_t p1= 0, p2= 0;
    	
		p2 = vb.find(':', p1);
    	c_name.assign(vb, 0, p2);
    	p1 = p2 + 1;
    	
		p2 = vb.find(':', p1);
		c_host.assign(vb, p1, p2 - p1);
    	p1 = p2 + 1;

    	p2 = vb.find(':', p1);
		c_port = atoi(vb.substr(p1, p2 - p1).c_str());
		p1 = p2 + 1;
    	    	
		p2 = vb.find(':', p1);
		c_username.assign(vb, p1, p2 - p1);
    	p1 = p2 + 1;
    	
		p2 = vb.find(':', p1);
		c_password.assign(vb, p1, p2 - p1);
    	p1 = p2 + 1;		
				
    	if (!(c_name.empty() || c_host.empty() || c_port == 0 || c_username.empty() || c_password.empty())) {
	 	
    		VBoxC[vboxes] = NULL;

	        cVBOXClient *v = new cVBOXClient(c_name.c_str(), c_host.c_str(), c_port, c_username.c_str(), c_password.c_str());
	        
	        if (v) {
	        	new_messages[vboxes] = 0;
	        	VBoxC[vboxes++] = v;
	        }
	    }
    }

    // Define Sizes and Layout :
    
    header_font = "Vera/" + conv::itos(resolution_dependant_font_size(28, conf->p_v_res()));
    normal_font = "Vera/" + conv::itos(resolution_dependant_font_size(17, conf->p_v_res()));
    
    s_header = string_format::calculate_string_size("abcltuwHPMjJg", header_font);
    header_box_size = static_cast<int>(s_header.second * 0.75);
	
	s_new 		= string_format::calculate_string_size("*", normal_font);					// Size of New-Flag
	s_time		= string_format::calculate_string_size("31.12.99 23:59:59", normal_font);	// Size of Time
	s_duration 	= string_format::calculate_string_size("59:59", normal_font);				// Size of Duration
	s_id 		= string_format::calculate_string_size("9999999999999999999", normal_font);	// Size of CallerId
	s_name 		= string_format::calculate_string_size("mmmmmmmmmmmmmmmmmmmmmmmmm", normal_font);		// Width of Name
	
    if (conf->p_h_res() < 1024) {
    	columns = 4;
    } else {
    	columns = 5;
    }
    
    font_height = s_new.second;
        
    width  = conf->p_h_res() - BORDER_LEFT - BORDER_RIGHT - 20;

    start_x = BORDER_LEFT + 10;
    subheader_y = BORDER_TOP + s_header.second;
    tableheader_y = subheader_y + font_height;
    start_y = tableheader_y + font_height;
    
    height = conf->p_v_res() - start_y - global->getBottomHeight() - SPACING;
    lines_to_display = int(height / font_height);
    height = (lines_to_display) * font_height;

    anchor = lines_to_display / 2;
    cursor = 0;
    first_row = 0;

    if (columns > 4) {
    	spacing			= ((width - 20) - (s_new.first + s_time.first + s_duration.first + s_id.first + s_name.first)) / (columns -1);
    } else {
    	spacing			= ((width - 20) - (s_new.first + s_time.first + s_duration.first + s_id.first)) / (columns -1);
    }
    
    if (spacing < 5) {
    	spacing = 5;
    }
    col_new 		= start_x;
    col_time 		= col_new + s_new.first + spacing;
    col_duration	= col_time + s_time.first + spacing;
    col_id			= col_duration + s_duration.first + spacing;
    col_name		= col_id + s_id.first + spacing;

}

#ifdef use_notify_area
void VBox::ShowNotifyArea()
{
	int x, y, n;
	int count;   
	
	if (intervall > INTERVALL) {
		n = num_vbox;
		for (num_vbox = 0; num_vbox < vboxes; num_vbox++) {
			Refresh();
		}
		num_vbox = n;
		intervall = 0;
	} else {
		intervall++;
	}

	count = 0;
	for (n=0; n < vboxes; n++) {
		count += new_messages[n];
	}
	
	if (count > 0) {
		x = conf->p_h_res() - NotifyArea::width/2 - 48/2;
		y = conf->p_v_res() - NotifyArea::height/2 - 48/2;
		render->image_mut.enterMutex();
		nArea->status_overlay.add(new PObj("./vbox/large_icons/notify.png", x, y, 1, NOSCALING));
		render->image_mut.leaveMutex();
	}
}
#endif

bool VBox::Connect()
{
	// printf("Connect %d\n", num_vbox);
  	if (!VBoxC[num_vbox]->IsConnected()) {
   		// printf("Connecting to vbox ...\n");
   		if (VBoxC[num_vbox]->Connect() == cVBOXClient::vboxSuccess) {
   			// printf("connected !\n");
   			return true;
   		}
   	}
  	return false;
}

void  VBox::Disconnect()
{
	if (VBoxC[num_vbox]->IsConnected()) {
		VBoxC[num_vbox]->Disconnect();
	}
}

std::string VBox::Toggle(int nr)
{
	if (Connect()) {
		if (VBoxC[num_vbox]->ToggleNewFlag(nr) == cVBOXClient::vboxSuccess) {
			Disconnect();
			return "";
		} else {
			Disconnect();
			return "failed";
		}
    } else {
    	return "failed";
    }
}

std::string VBox::Delete(int nr)
{
	if (Connect()) {
		if (VBoxC[num_vbox]->ToggleDeletedFlag(nr) == cVBOXClient::vboxSuccess) {
			Disconnect();
			return "";
		} else {
			Disconnect();
			return "failed";
		}
    } else {
    	return "failed";
    }
}

bool VBox::IsNew(int nr)
{
	if (Connect()) {
		if (VBoxC[num_vbox]->GetMsgIsNew(nr, &is_new) == cVBOXClient::vboxSuccess) {
			Disconnect();
			// printf("New !\n");
			return is_new;
		} else {
			// printf("Old !\n");
			Disconnect();
			return false;
		}
    } else {
		// printf("Not Connected !\n");
    	return false;
    }
}

bool VBox::CheckDelete()
{
	if (Connect()) {
		if (VBoxC[num_vbox]->GetMsgDeletedCount() > 0) {
			ExtraMenu em(dgettext("mms-vbox", "Delete marked Messages ?"));

			    em.add_item(ExtraMenuItem(dgettext("mms-vbox", "Yes"), "", NULL));
			em.add_item(ExtraMenuItem(dgettext("mms-vbox", "No"), "", NULL));

			if (em.mainloop() == 0) {
			    VBoxC[num_vbox]->DeleteMarkedMessages();
			}
		}
		Disconnect();
		return true;
    } else {
		// printf("Not Connected !\n");
    	return false;
    }
}

std::string VBox::GetRecording(int nr)
{
	if (Connect()) {
		if (VBoxC[num_vbox]->CreateAuFileFromMsg(nr, vbox_conf->p_vbox_temp_file().c_str(), vbox_conf->p_vbox_conv_tool().c_str()) == cVBOXClient::vboxSuccess) {
//			printf("Success !\n");
			Disconnect();
			return "";
		} else {
//			printf("No Success !\n");
			Disconnect();
			return "failed";
		}
    }
   	return "failed";
}

std::string VBox::Refresh()
{
	new_messages[num_vbox] = 0;
  	if (Connect()) {
 	    if (VBoxC[num_vbox]->TransferMsgList() == cVBOXClient::vboxSuccess) {
	        number_of_messages = VBoxC[num_vbox]->GetMsgTotalCount();
	    	new_messages[num_vbox] = VBoxC[num_vbox]->GetMsgNewCount();
	    	Disconnect();
	    	return "";
	    } else {
	    	Disconnect();
	    	return "failed";
	    }
  	}
  	return "failed";
}

void VBox::GetData(int nr)
{
	VBoxC[num_vbox]->GetMsgIsNew(nr, &is_new);
	VBoxC[num_vbox]->GetMsgTime(nr, &time);
	strftime(buffer, 255, "%d.%m.%y %X", &time);
	timestr.assign(buffer);
	
	b = &buffer[0];
	VBoxC[num_vbox]->GetMsgPlaytime(nr, &b);
	duration.assign(b);
	b = &buffer[0];
	VBoxC[num_vbox]->GetMsgCallerId(nr, &b);
	if (b != NULL) {
		callerid.assign(b);
	} else {
		callerid.assign(dgettext("mms-vbox", "*** Unknown ***"));
	}
	b = &buffer[0];
 	VBoxC[num_vbox]->GetMsgCallerName(nr, &b);		
 	if (b != NULL) {
		name.assign(b);
	} else {
		name.assign(dgettext("mms-vbox", "*** Unknown ***"));
	}
}

void VBox::BeginAnimation()
{
    render->device->animation_section_begin();
    curlayer = render->device->get_current_layer();
    render->device->animation_section_end();
}

void VBox::EndAnimation()
{
    render->device->animation_section_begin();
    render->device->switch_to_layer(curlayer+1);
    render->device->reset_layout_attribs_nowait();
    render->device->animation_zoom(0.90,0.90,1,1,40,curlayer+1);
    render->device->animation_fade(0,1,50,curlayer+1);
    render->device->animation_fade(1,0,60,curlayer);
    render->device->animation_section_end();

}

std::string VBox::Print(std::string status)
{
    
    render->prepare_new_image();
    

	if (animation)  {
		BeginAnimation();
	}

    render->current.add(new PObj(themes->vbox_background, 0, 0, 0, SCALE_FULL));

    PObj *icon = new PFObj(themes->startmenu_vbox, BORDER_LEFT, BORDER_TOP, header_box_size, header_box_size, 2, true);

    S_Touch::get_instance()->register_area(TouchArea(rect(icon->x, icon->y, icon->w, icon->h), icon->layer,
  						   boost::bind(&VBox::exit, this)));
    
    render->current.add(icon);

    render->current.add(new RObj(start_x - 10, start_y, width + 20, height,
					 themes->general_rectangle_color1, themes->general_rectangle_color2,
					 themes->general_rectangle_color3, themes->general_rectangle_color4, 1));
	    
    if (themes->show_header) {
		    sprintf(buffer, "%s %s", dgettext("mms-vbox", "VBox"), VBoxC[num_vbox]->Name());
		    buf.assign(buffer);
		    render->current.add(new TObj(buf, header_font, BORDER_LEFT + header_box_size + 10, BORDER_TOP + (header_box_size - s_header.second)/2,
		                  themes->vbox_header_font1, themes->vbox_header_font2,
		                  themes->vbox_header_font3, 3));
    }
    
	if (status == "failed") {
		render->current.add(new TObj(dgettext("mms-vbox", "Could not connect to"),
		              normal_font, col_time, start_y,
		              themes->vbox_header_font1,
		              themes->vbox_header_font2,
		              themes->vbox_header_font3, 3) );
		
	    sprintf(buffer, "%s : %s, %s : %d", dgettext("mms-vbox", "Host"), VBoxC[num_vbox]->VBOXServer(), dgettext("mms-vbox", "Port"), VBoxC[num_vbox]->Port());
	    buf.assign(buffer);    
		render->current.add(new TObj(buf,
				              normal_font, col_time, start_y + font_height,
				              themes->vbox_header_font1,
				              themes->vbox_header_font2,
				              themes->vbox_header_font3, 3) );
		
	    sprintf(buffer, "%s : %s, %s : %s", dgettext("mms-vbox", "Username"), VBoxC[num_vbox]->Username(), dgettext("mms-vbox", "Password"), VBoxC[num_vbox]->Password());
	    buf.assign(buffer);    
		render->current.add(new TObj(buf,
				              normal_font, col_time, start_y + font_height + font_height,
				              themes->vbox_header_font1,
				              themes->vbox_header_font2,
				              themes->vbox_header_font3, 3) );
	} else {
	    if (themes->show_header) {
		    sprintf(buffer, "%d %s / %d %s", new_messages[num_vbox], dgettext("mms-vbox", "New"), number_of_messages, dgettext("mms-vbox", "Total"));
		    buf.assign(buffer);
			
			s_status 		= string_format::calculate_string_size(buf, normal_font);		// Size of Status
	 
			render->current.add(new TObj(buf,
			              normal_font, start_x, subheader_y,
			              themes->vbox_header_font1,
			              themes->vbox_header_font2,
			              themes->vbox_header_font3, 3) );
	
	    }
	
	    render->current.add(new TObj(dgettext("mms-vbox", "Time"),
	                  normal_font, col_time, tableheader_y,
	                  themes->vbox_header_font1,
	                  themes->vbox_header_font2,
	                  themes->vbox_header_font3, 3) );
		        
	
	    render->current.add(new TObj(dgettext("mms-vbox", "Min"),
	                  normal_font, col_duration, tableheader_y,
	                  themes->vbox_header_font1,
	                  themes->vbox_header_font2,
	                  themes->vbox_header_font3, 2) );
	
	    render->current.add(new TObj(dgettext("mms-vbox", "CallerId"),
	                  normal_font, col_id, tableheader_y,
	                  themes->vbox_header_font1,
	                  themes->vbox_header_font2,
	                  themes->vbox_header_font3, 3) );
	
	    if (columns > 4) {
		    render->current.add(new TObj(dgettext("mms-vbox", "Name"),
		                  normal_font, col_name, tableheader_y,
		                  themes->vbox_header_font1,
		                  themes->vbox_header_font2,
		                  themes->vbox_header_font3, 3) );
	    }
	
	    for (i = 0; i < lines_to_display && i < number_of_messages; i++) {
	
	    	buf.assign("");
			VBoxC[num_vbox]->GetMsgIsNew(i + first_row, &is_new);
			VBoxC[num_vbox]->GetMsgIsDeleted(i + first_row, &is_marked_deleted);

			if (is_marked_deleted) {
				buf.insert(0, "-");
			} else if (is_new) {
				buf.insert(0, "*");
			} else {
				buf.insert(0, "");
			}
	  		render->current.add(new TObj(buf,
			              normal_font, col_new, start_y + (font_height * i),
			              themes->vbox_font1,
			              themes->vbox_font2,
			              themes->vbox_font3, 3) );
	
	    	VBoxC[num_vbox]->GetMsgTime(i + first_row, &time);
			strftime(buffer, 255, "%d.%m.%y %X", &time);
			buf.assign(buffer);
	  		render->current.add(new TObj(buf,
			              normal_font, col_time, start_y + (font_height * i),
			              themes->vbox_font1,
			              themes->vbox_font2,
			              themes->vbox_font3, 3) );
	
	  		b = &buffer[0];
	  		VBoxC[num_vbox]->GetMsgPlaytime(i + first_row, &b);
			buf.assign(b);
			render->current.add(new TObj(buf,
			              normal_font, col_duration, start_y + (font_height * i),
			              themes->vbox_font1,
			              themes->vbox_font2,
			              themes->vbox_font3, 3) );
			
	  		b = &buffer[0];
	  		VBoxC[num_vbox]->GetMsgCallerId(i + first_row, &b);
			if (b != NULL) {
				buf.assign(b);
			} else {
				buf.assign(dgettext("mms-vbox", "*** Unknown ***"));
			}
			render->current.add(new TObj(buf,
			              normal_font, col_id, start_y + (font_height * i),
			              themes->vbox_font1,
			              themes->vbox_font2,
			              themes->vbox_font3, 3) );
			
		    if (columns > 4) {
		  		b = &buffer[0];
		 		VBoxC[num_vbox]->GetMsgCallerName(i + first_row, &b);		
		
		 		if (b != NULL) {
					buf.assign(b);
				} else {
					buf.assign(dgettext("mms-vbox", "*** Unknown ***"));
				}
		 		string_format::format_to_size(buf, normal_font, width - col_name, true);

		 		render->current.add(new TObj(buf,
				              normal_font, col_name, start_y + (font_height * i),
				              themes->vbox_font1,
				              themes->vbox_font2,
				              themes->vbox_font3, 3) );
		
			}
			S_Touch::get_instance()->register_area(TouchArea(rect(col_new, start_y + (font_height * i), width, font_height), 3,
											boost::bind(&VBox::TouchandPlay, this, i)));
	    }

	    render->current.add(new PFObj(themes->general_marked, start_x - 10, start_y + (font_height * cursor), width + 20, font_height, 2, true));
	}

	if (animation)  {
		EndAnimation();
	}
    render->draw_and_release("mms-vbox");

    return "";
}

void VBox::toggle_new()
{
	Toggle(cursor + first_row);
}

void VBox::toggle_delete()
{
	Delete(cursor + first_row);
}

void VBox::exit()
{
	printf("exit\n");
    exit_loop=true;
}

std::string VBox::TouchandPlay(int offset) {
	cursor = offset;
	Print("");
	return Play();
}

std::string VBox::Play() {
	// printf("Play Message\n");
	if (IsNew(first_row + cursor)) {
    	// printf("Toggle unread\n");
    	if (Toggle(first_row + cursor) == "failed") {
    		return "failed";
    	}
	}
	if (GetRecording(first_row + cursor) == "failed") {
		// printf("Recording failed !\n");
		return "failed";
	} else {
    	GetData(first_row + cursor);
		sprintf(buffer, "%s %s %s %s", dgettext("mms-vbox", "Call from"), callerid.c_str(), dgettext("mms-vbox", "at"), timestr.c_str());
	    buf.assign(buffer);
    	
		cur_track.id = 4711;
		cur_track.name = buf;
		cur_track.path = vbox_conf->p_vbox_temp_file().c_str(); // "/tmp/mms-vbox.wav";
		cur_track.type = "media-file";
		
		// printf("%s\n", buf.c_str());
	    if (as->p_playing()) {
	    	as->suspend_normal_audio_and_play_track(cur_track);
	    } else {
    		as->external_plugin_playback(cur_track);
	    }
		// printf("bla\n");
	}
    return "";
}

std::string VBox::mainloop()
{
    exit_loop=false;
    bool update_needed=false;
    bool failed=false;

    
    BusyIndicator *busy_indicator = S_BusyIndicator::get_instance();

	input_master->save_map();
	input_master->set_map("vbox");

	animation = true;
    Print(Refresh());
    animation = false;
	
    Input input;

    // main input loop
    while (!exit_loop) {

    	if (update_needed) {

            update_needed=false;

            Print(Refresh());
            
            animation = false;
        }
        input = input_master->get_input_busy_wrapped(busy_indicator);
    
        if (input.key == "touch_input") {
          S_Touch::get_instance()->run_callback();
          update_needed = true;
          continue;
        }
        
        if (input.command == "back" || failed == true ) {
            exit_loop=true;
        } else if (input.command == "startmenu") {
            exit_loop = true;
        } else if (input.command == "prev") {
        	// printf("up\n");
        	if (cursor < anchor) {
        		// Cursor ist in oberer Hälfte
        		if (first_row > 0) {
        			first_row--;
            		update_needed = true;
        		} else if (cursor > 0) {
        			cursor--;
            		update_needed = true;
        		}
        	} else {
        		cursor--;
        		update_needed = true;
        	}
        } else if (input.command == "next") {
        	// printf("down\n");
        	if (cursor > anchor) {
        		// Cursor ist in unterer Hälfte 
        		if (first_row + lines_to_display < number_of_messages) {
        			first_row++;
            		update_needed = true;
        		} else if (first_row + cursor < (number_of_messages - 1)){
        			cursor++;
            		update_needed = true;
        		}
        	} else if (cursor < number_of_messages - 1){
        		cursor++;
        		update_needed = true;
        	}
        } else if (input.command == "page_up") {
        	// printf("page up %d %d\n", num_vbox, vboxes);
        	CheckDelete();
        	if (num_vbox > 0) {
        		num_vbox--;	
        	} else {
        		num_vbox = vboxes - 1;
        	}
        	first_row = 0;
        	cursor = 0;
        	animation = true;
        	update_needed = true;
        } else if (input.command == "page_down") {
        	// printf("page down %d %d\n", num_vbox, vboxes);
        	CheckDelete();
        	if (num_vbox < vboxes - 1) {
        		num_vbox++;
        	} else {
        		num_vbox=0;
        	}
        	first_row = 0;
        	cursor = 0;
        	animation = true;
        	update_needed = true;
        } else if (input.command == "left") {
        	// printf("left\n");
        } else if (input.command == "right") {
        	// printf("right\n");
        } else if (input.command == "action") {
        	if (Play() == "failed") {
            		return "failed";
            	}
        		
        	update_needed = true;
    	} else if (input.command == "second_action") {
			ExtraMenu em;
			em.add_item(ExtraMenuItem(dgettext("mms-vbox", "Togggle New-Flag"),
					input_master->find_shortcut("toggle_new"), boost::bind(
							&VBox::toggle_new, this)));

			em.add_item(ExtraMenuItem(dgettext("mms-vbox", "Toggle Delete-Flag"),
					input_master->find_shortcut("toggle_delete"), boost::bind(
							&VBox::toggle_delete, this)));

			foreach (ExtraMenuItem& item, global->menu_items)
        		em.add_persistent_item(item);

        	em.mainloop();
        	update_needed = true;
    	} else if (input.command == "toggle_new") {
        	// printf("Toggle read/unread\n");
        	if (Toggle(cursor + first_row) == "failed") {
        		return "failed";
        	}
        	update_needed = true;
    	} else if (input.command == "toggle_delete") {
        	// printf("Toggle delete-flag\n");
        	if (Delete(cursor + first_row) == "failed") {
        		return "failed";
        	}
        	update_needed = true;
        } else {
            update_needed = !global->check_commands(input);
        }
    }
	CheckDelete();

    render->device->animation_section_begin();
    render->device->switch_to_layer(curlayer);
    render->device->reset_layout_attribs_nowait();
    render->device->animation_fade(0,1,80,curlayer);
    render->device->animation_zoom(1,1,4,4,40,curlayer+1);
    render->device->animation_fade(1,0,40,curlayer+1);
    render->device->animation_section_end();

    input_master->restore_map();
    return "";
}
