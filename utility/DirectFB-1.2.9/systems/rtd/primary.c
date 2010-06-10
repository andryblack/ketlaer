/*
   (c) Copyright 2001-2008  The world wide DirectFB Open Source Community (directfb.org)
   (c) Copyright 2000-2004  Convergence (integrated media) GmbH

   All rights reserved.

   Written by Denis Oliver Kropp <dok@directfb.org>,
              Andreas Hundt <andi@fischlustig.de>,
              Sven Neumann <neo@directfb.org>,
              Ville Syrjälä <syrjala@sci.fi> and
              Claudio Ciccani <klan@users.sf.net>.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the
   Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#include <directfb.h>

#include <fusion/fusion.h>
#include <fusion/shmalloc.h>

#include <core/core.h>
#include <core/coredefs.h>
#include <core/coretypes.h>
#include <core/layers.h>
#include <core/palette.h>
#include <core/surface.h>
#include <core/system.h>
#include <core/input.h>

#include <gfx/convert.h>

#include <misc/conf.h>

#include <direct/memcpy.h>
#include <direct/messages.h>
#include <direct/thread.h>

#include "libk.h"
#include "rtd.h"
#include "primary.h"


static DFBResult dfb_rtd_set_video_mode( CoreDFB* core, CoreLayerRegionConfig *config );
static DFBResult dfb_rtd_update_screen( CoreDFB *core, DFBRegion *region );
static DFBResult dfb_rtd_set_palette( CorePalette *palette );

static DFBResult update_screen( CoreSurface *surface,
                                int x, int y, int w, int h );

extern DFBRTD *dfb_rtd;
extern CoreDFB *dfb_rtd_core;
static CoreInputDevice *rtdInputDevice;

/******************************************************************************/

static DFBResult
primaryInitScreen( CoreScreen           *screen,
                   CoreGraphicsDevice   *device,
                   void                 *driver_data,
                   void                 *screen_data,
                   DFBScreenDescription *description )
{
     /* Set the screen capabilities. */
     description->caps = DSCCAPS_NONE;

     /* Set the screen name. */
     snprintf( description->name,
               DFB_SCREEN_DESC_NAME_LENGTH, "RTD Primary Screen" );

     return DFB_OK;
}

static DFBResult
primaryGetScreenSize( CoreScreen *screen,
                      void       *driver_data,
                      void       *screen_data,
                      int        *ret_width,
                      int        *ret_height )
{
  D_ASSERT( dfb_rtd != NULL );

  libk_getscreensize(ret_width, ret_height);
  return DFB_OK;
}

ScreenFuncs rtdPrimaryScreenFuncs = {
     .InitScreen    = primaryInitScreen,
     .GetScreenSize = primaryGetScreenSize,
};

/******************************************************************************/

static int
primaryLayerDataSize( void )
{
     return 0;
}

static int
primaryRegionDataSize( void )
{
     return 0;
}

static DFBResult
primaryInitLayer( CoreLayer                  *layer,
                  void                       *driver_data,
                  void                       *layer_data,
                  DFBDisplayLayerDescription *description,
                  DFBDisplayLayerConfig      *config,
                  DFBColorAdjustment         *adjustment )
{
     D_DEBUG( "DirectFB/RTD: primaryInitLayer\n");
          
     /* set capabilities and type */
     description->caps = DLCAPS_SURFACE;
     description->type = DLTF_GRAPHICS;

     /* set name */
     snprintf( description->name,
               DFB_DISPLAY_LAYER_DESC_NAME_LENGTH, "RTD Primary Layer" );

     /* fill out the default configuration */
     config->flags       = DLCONF_WIDTH       | DLCONF_HEIGHT |
                           DLCONF_PIXELFORMAT | DLCONF_BUFFERMODE;
     config->buffermode  = DLBM_FRONTONLY;

     if (dfb_config->mode.width)
          config->width  = dfb_config->mode.width;
     else
          config->width  = 640;

     if (dfb_config->mode.height)
          config->height = dfb_config->mode.height;
     else
          config->height = 480;

     if (dfb_config->mode.format != DSPF_UNKNOWN)
          config->pixelformat = dfb_config->mode.format;
     else if (dfb_config->mode.depth > 0)
          config->pixelformat = dfb_pixelformat_for_depth( dfb_config->mode.depth );
     else
          config->pixelformat = DSPF_RGB24;

     return DFB_OK;
}

static DFBResult
primaryTestRegion( CoreLayer                  *layer,
                   void                       *driver_data,
                   void                       *layer_data,
                   CoreLayerRegionConfig      *config,
                   CoreLayerRegionConfigFlags *failed )
{
     CoreLayerRegionConfigFlags fail = 0;

     switch (config->buffermode) {
          case DLBM_FRONTONLY:
          case DLBM_BACKSYSTEM:
          case DLBM_BACKVIDEO:
               break;

          default:
               fail |= CLRCF_BUFFERMODE;
               break;
     }

     if (config->options)
          fail |= CLRCF_OPTIONS;

     if (failed)
          *failed = fail;

     if (fail)
          return DFB_UNSUPPORTED;

     return DFB_OK;
}

static DFBResult
primaryAddRegion( CoreLayer             *layer,
                  void                  *driver_data,
                  void                  *layer_data,
                  void                  *region_data,
                  CoreLayerRegionConfig *config )
{
     return DFB_OK;
}

static DFBResult
primarySetRegion( CoreLayer                  *layer,
                  void                       *driver_data,
                  void                       *layer_data,
                  void                       *region_data,
                  CoreLayerRegionConfig      *config,
                  CoreLayerRegionConfigFlags  updated,
                  CoreSurface                *surface,
                  CorePalette                *palette,
                  CoreSurfaceBufferLock      *lock )
{
     DFBResult ret;

     D_DEBUG( "DirectFB/RTD: primarySetRegion\n");

     ret = dfb_rtd_set_video_mode( dfb_rtd_core, config );
     if (ret)
          return ret;

     if (surface)
          dfb_rtd->primary = surface;

     if (palette)
          dfb_rtd_set_palette( palette );

#if 0
     driver_data = (void*) rfb_screen; 
#endif
     
     return DFB_OK;
}

static DFBResult
primaryRemoveRegion( CoreLayer             *layer,
                     void                  *driver_data,
                     void                  *layer_data,
                     void                  *region_data )
{
     dfb_rtd->primary = NULL;

     return DFB_OK;
}

static DFBResult
primaryFlipRegion( CoreLayer             *layer,
                   void                  *driver_data,
                   void                  *layer_data,
                   void                  *region_data,
                   CoreSurface           *surface,
                   DFBSurfaceFlipFlags    flags,
                   CoreSurfaceBufferLock *lock )
{
     dfb_surface_flip( surface, false );

     return dfb_rtd_update_screen( dfb_rtd_core, NULL );
}

static DFBResult
primaryUpdateRegion( CoreLayer             *layer,
                     void                  *driver_data,
                     void                  *layer_data,
                     void                  *region_data,
                     CoreSurface           *surface,
                     const DFBRegion       *update,
                     CoreSurfaceBufferLock *lock )
{
     if (update) {
          DFBRegion region = *update;

          return dfb_rtd_update_screen( dfb_rtd_core, &region );
     }

     return dfb_rtd_update_screen( dfb_rtd_core, NULL );
}

static DFBResult
primaryAllocateSurface( CoreLayer              *layer,
                        void                   *driver_data,
                        void                   *layer_data,
                        void                   *region_data,
                        CoreLayerRegionConfig  *config,
                        CoreSurface           **ret_surface )
{
     CoreSurfaceConfig conf;

     conf.flags  = CSCONF_SIZE | CSCONF_FORMAT | CSCONF_CAPS;
     conf.size.w = config->width;
     conf.size.h = config->height;
     conf.format = config->format;
     conf.caps   = DSCAPS_SYSTEMONLY;

     if (config->buffermode != DLBM_FRONTONLY)
          conf.caps |= DSCAPS_DOUBLE;

     return dfb_surface_create( dfb_rtd_core, &conf, CSTF_LAYER, DLID_PRIMARY, NULL, ret_surface );
}

static DFBResult
primaryReallocateSurface( CoreLayer             *layer,
                          void                  *driver_data,
                          void                  *layer_data,
                          void                  *region_data,
                          CoreLayerRegionConfig *config,
                          CoreSurface           *surface )
{
     DFBResult         ret;
     CoreSurfaceConfig conf;

     conf.flags  = CSCONF_SIZE | CSCONF_FORMAT | CSCONF_CAPS;
     conf.size.w = config->width;
     conf.size.h = config->height;
     conf.format = config->format;
     conf.caps   = DSCAPS_SYSTEMONLY;

     if (config->buffermode != DLBM_FRONTONLY)
          conf.caps |= DSCAPS_DOUBLE;

     ret = dfb_surface_reconfig( surface, &conf );
     if (ret)
          return ret;

     if (DFB_PIXELFORMAT_IS_INDEXED(config->format) && !surface->palette) {
          DFBResult    ret;
          CorePalette *palette;

          ret = dfb_palette_create( NULL,    /* FIXME */
                                    1 << DFB_COLOR_BITS_PER_PIXEL( config->format ),
                                    &palette );
          if (ret)
               return ret;

          if (config->format == DSPF_LUT8)
               dfb_palette_generate_rgb332_map( palette );

          dfb_surface_set_palette( surface, palette );

          dfb_palette_unref( palette );
     }

     return DFB_OK;
}

DisplayLayerFuncs rtdPrimaryLayerFuncs = {
     .LayerDataSize     = primaryLayerDataSize,
     .RegionDataSize    = primaryRegionDataSize,
     .InitLayer         = primaryInitLayer,

     .TestRegion        = primaryTestRegion,
     .AddRegion         = primaryAddRegion,
     .SetRegion         = primarySetRegion,
     .RemoveRegion      = primaryRemoveRegion,
     .FlipRegion        = primaryFlipRegion,
     .UpdateRegion      = primaryUpdateRegion,

     .AllocateSurface   = primaryAllocateSurface,
     .ReallocateSurface = primaryReallocateSurface,
};

/******************************************************************************/

static DFBResult
update_screen( CoreSurface *surface, int x, int y, int w, int h )
{
     DFBResult              ret;
     void                  *src;
     CoreSurfaceBufferLock  lock;

     D_ASSERT( surface != NULL );

     ret = dfb_surface_lock_buffer( surface, CSBR_FRONT, CSAF_CPU_READ, &lock );
     if (ret) {
          D_DERROR( ret, "DirectFB/RTD: Couldn't lock layer surface!\n" );
          return ret;
     }

     src = lock.addr; /* + DFB_BYTES_PER_LINE( surface->config.format, x ) + y * lock.pitch;*/

     switch(surface->config.format) {
     case DSPF_RGB16:
       libk_copyrect_16(src, lock.pitch, x, y, w, h);
       break;
     case DSPF_RGB32:
     case DSPF_ARGB:
       libk_copyrect_32(src, lock.pitch, x, y, w, h);
       break;
     }

     dfb_surface_unlock_buffer( surface, &lock );

     return DFB_OK;
}

/******************************************************************************/

typedef enum {
     RTD_SET_VIDEO_MODE,
     RTD_UPDATE_SCREEN,
     RTD_SET_PALETTE
} DFBRTDCall;

static DFBResult
dfb_rtd_set_video_mode_handler( CoreLayerRegionConfig *config )
{
#if 0
     int argc = 0;
     char** argv = NULL;

     D_DEBUG( "DirectFB/RTD: layer config properties\n");

     if(rfb_screen) /*!!! FIXME*/
         return DFB_OK;

     fusion_skirmish_prevail( &dfb_rtd->lock );

     /* Set video mode */
     rfb_screen = rfbGetScreen(&argc, argv, config->width, config->height, DFB_BITS_PER_PIXEL(config->format)/3, 3, 4);
     
     D_DEBUG( "DirectFB/RTD: rfbGetScreen parameters: width %d height %d bitspersample %d samplesperpixel %d bytesperpixel %d\n", config->width, config->height, DFB_BITS_PER_PIXEL(config->format)/3, 3, 4);
     
     /*screen = rfbGetScreen(&argc, argv, config->width, config->height, 8, 3, 4);*/

     if ( rfb_screen == NULL )
     {
             D_ERROR( "DirectFB/RTD: Couldn't set %dx%dx%d video mode\n",
                      config->width, config->height,
                      DFB_COLOR_BITS_PER_PIXEL(config->format) );

             fusion_skirmish_dismiss( &dfb_rtd->lock );

             return DFB_FAILURE;
     }

     if(DFB_COLOR_BITS_PER_PIXEL(config->format) == DSPF_RGB16)
     {
        rfb_screen->serverFormat.redShift = 11;
        rfb_screen->serverFormat.greenShift = 5;
        rfb_screen->serverFormat.blueShift = 0;
        rfb_screen->serverFormat.redMax = 31;
        rfb_screen->serverFormat.greenMax = 63;
        rfb_screen->serverFormat.blueMax = 31;
     }
    
     /* screen->serverFormat.trueColour=FALSE; */

     rfb_screen->frameBuffer = malloc(rfb_screen->width * rfb_screen->height * rfb_screen->depth / 8) ;
     
     if ( ! rfb_screen->frameBuffer )
     {
             fusion_skirmish_dismiss( &dfb_rtd->lock );

             return DFB_NOSYSTEMMEMORY;
     }

     fusion_skirmish_dismiss( &dfb_rtd->lock );
#endif
     return DFB_OK;
}

static DFBResult
dfb_rtd_update_screen_handler( DFBRegion *region )
{
     DFBResult    ret;
     CoreSurface *surface;
      
     D_ASSERT(dfb_rtd);
      
     surface = dfb_rtd->primary;

     fusion_skirmish_prevail( &dfb_rtd->lock );

     if (!region)
          ret = update_screen( surface, 0, 0, surface->config.size.w, surface->config.size.h );
     else
          ret = update_screen( surface,
                               region->x1,  region->y1,
                               region->x2 - region->x1 + 1,
                               region->y2 - region->y1 + 1 );

     fusion_skirmish_dismiss( &dfb_rtd->lock );

     return DFB_OK;
}

static DFBResult
dfb_rtd_set_palette_handler( CorePalette *palette )
{
#if 0
     unsigned int i;
     uint8_t* map;

     rfb_screen->colourMap.count = palette->num_entries;
     rfb_screen->colourMap.is16 = false;
     rfb_screen->serverFormat.trueColour=FALSE;

     D_DEBUG( "DirectFB/RTD: setting colourmap of size %d\n", palette->num_entries);
     
     if( (map = (uint8_t*) malloc(rfb_screen->colourMap.count*sizeof(uint8_t)*3)) == NULL )
          return DFB_NOSYSTEMMEMORY;

     for (i=0; i<palette->num_entries; i++) {
          *(map++) = palette->entries[i].r;
          *(map++) = palette->entries[i].g;
          *(map++) = palette->entries[i].b;
     }

     fusion_skirmish_prevail( &dfb_rtd->lock );

     if( rfb_screen->colourMap.data.bytes )
          free(rfb_screen->colourMap.data.bytes);
     rfb_screen->colourMap.data.bytes = map;

     fusion_skirmish_dismiss( &dfb_rtd->lock );

#endif
     return DFB_OK;
}

FusionCallHandlerResult
dfb_rtd_call_handler( int           caller,
                      int           call_arg,
                      void         *call_ptr,
                      void         *ctx,
                      unsigned int  serial,
                      int          *ret_val )
{
     switch (call_arg) {
          case RTD_SET_VIDEO_MODE:
               *ret_val = dfb_rtd_set_video_mode_handler( call_ptr );
               break;

          case RTD_UPDATE_SCREEN:
               *ret_val = dfb_rtd_update_screen_handler( call_ptr );
               break;

          case RTD_SET_PALETTE:
               *ret_val = dfb_rtd_set_palette_handler( call_ptr );
               break;

          default:
               D_BUG( "unknown call" );
               *ret_val = DFB_BUG;
               break;
     }

     return FCHR_RETURN;
}

static DFBResult
dfb_rtd_set_video_mode( CoreDFB *core, CoreLayerRegionConfig *config )
{
     int                    ret;
     CoreLayerRegionConfig *tmp = NULL;

     D_ASSERT( config != NULL );

     if (dfb_core_is_master( core ))
          return dfb_rtd_set_video_mode_handler( config );

     if (!fusion_is_shared( dfb_core_world(core), config )) {
          tmp = SHMALLOC( dfb_core_shmpool(core), sizeof(CoreLayerRegionConfig) );
          if (!tmp)
               return D_OOSHM();

          direct_memcpy( tmp, config, sizeof(CoreLayerRegionConfig) );
     }

     fusion_call_execute( &dfb_rtd->call, FCEF_NONE, RTD_SET_VIDEO_MODE,
                          tmp ? tmp : config, &ret );

     if (tmp)
          SHFREE( dfb_core_shmpool(core), tmp );

     return ret;
}

static DFBResult
dfb_rtd_update_screen( CoreDFB *core, DFBRegion *region )
{
     int        ret;
     DFBRegion *tmp = NULL;

     if (dfb_core_is_master( core ))
          return dfb_rtd_update_screen_handler( region );

     if (region) {
          if (!fusion_is_shared( dfb_core_world(core), region )) {
               tmp = SHMALLOC( dfb_core_shmpool(core), sizeof(DFBRegion) );
               if (!tmp)
                    return D_OOSHM();

               direct_memcpy( tmp, region, sizeof(DFBRegion) );
          }
     }

     fusion_call_execute( &dfb_rtd->call, FCEF_NONE, RTD_UPDATE_SCREEN,
                          tmp ? tmp : region, &ret );

     if (tmp)
          SHFREE( dfb_core_shmpool(core), tmp );

     return DFB_OK;
}

static DFBResult
dfb_rtd_set_palette( CorePalette *palette )
{
     int ret;

     fusion_call_execute( &dfb_rtd->call, FCEF_NONE, RTD_SET_PALETTE,
                          palette, &ret );

     return ret;
}
