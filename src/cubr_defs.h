// Copyright 2018 Nicolaus Anderson

#ifndef _CUBR_TYPES_H_
#define _CUBR_TYPES_H_

	// From Irrlicht
#include <rect.h>
#include <IImage.h>
#include <ITexture.h>
#include <IVideoDriver.h>
#include <IGUIElement.h>
#include <IGUIEnvironment.h>

namespace cubr {

	typedef  irr::core::recti  rect_t;
	typedef  irr::gui::IGUIElement  gui_element_t;
	typedef  irr::gui::IGUIEnvironment  gui_environment_t;
	typedef  irr::video::IVideoDriver  video_driver_t;
	typedef  irr::video::IImage  image_t;
	typedef  irr::video::ITexture  texture_t;

}

#endif
