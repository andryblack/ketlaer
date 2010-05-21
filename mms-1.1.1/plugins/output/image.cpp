#include "image.hpp"

#include "renderer.hpp"

// vector_lookup
#include "common.hpp"

#include <cassert>

using std::vector;

Image::Image()
{
  gelements empty;
  // 10 empty layers for normal objects
  // and 10 for overlay
  for (int i = 0; i < 20; ++i)
    elements.push_back(empty);
}

Image::~Image()
{
  if (image_data != NULL) {
    imlib_context_set_image(image_data);
    imlib_free_image();
  }
}

void Image::clear()
{
  int layer = 0;

  foreach (gelements& element, elements) {
    if (layer < 10)
      foreach (GObj *obj, element)
	delete obj;
    ++layer;
  }

  clear_no_delete();
}

void Image::clear_no_delete()
{
  int layer = 0;

  foreach (gelements& element, elements) {
    if (layer < 10)
      element.clear();
    ++layer;
  }
}

void Image::clone(const layers& l)
{
  layers::iterator eiter = elements.begin();

  foreach (gelements element, l) {
    foreach (GObj *obj, element)
      eiter->push_back(obj->clone());
    ++eiter;
  }
}

void Image::add(GObj *g)
{
  assert(g->layer < 10);

  // insert back into the vector, FIXME: might be slow
  elements[g->layer].push_back(g);
}

void Image::overlay_add(GObj *g)
{
  assert(g->layer > 9);

  // insert back into the vector, FIXME: might be slow
  elements[g->layer].push_back(g);
}

void Overlay::add(GObj *g)
{
  // overlay
  g->layer += 10;

  elements.push_back(g);

  Render *render = S_Render::get_instance();
  render->current.overlay_add(g);
}

void Overlay::cleanup()
{
  partial_cleanup();

  // draw = remove the overlay from screen
  Render *render = S_Render::get_instance();
  render->draw("remove overlay:" + name, true);

  partial_cleanup_final();
}

void Overlay::partial_cleanup()
{
  Render *render = S_Render::get_instance();

  if (temp_elements.size() > 0)
    partial_cleanup_final();

  // remove from current
  foreach (GObj *obj, elements) {
    Image::layers::iterator liter = render->current.elements.begin() + obj->layer;
    for (Image::gelements::iterator ogiter = liter->begin(),
	   ogend = liter->end(); ogiter != ogend; ++ogiter)
      if (obj->op_eq(*(*ogiter))) {
	liter->erase(ogiter);
	break;
      }
  }

  // add to previous
  foreach (GObj *obj, elements)
    render->previous.overlay_add(obj);

  temp_elements = elements;
  elements.clear();
}

void Overlay::partial_cleanup_final()
{
  Render *render = S_Render::get_instance();

  // remove from previous since the overlay has been removed
  foreach (GObj *obj, temp_elements) {
    Image::layers::iterator liter = render->previous.elements.begin() + obj->layer;
    for (Image::gelements::iterator ogiter = liter->begin(),
	   ogend = liter->end(); ogiter != ogend; ++ogiter)
      if (obj->op_eq(*(*ogiter))) {
	liter->erase(ogiter);
	break;
      }
  }

  foreach (GObj *obj, temp_elements)
    delete obj;

  temp_elements.clear();
}
