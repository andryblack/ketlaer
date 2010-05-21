#ifndef COMMON_OUTPUT_HPP
#define COMMON_OUTPUT_HPP

// overlay
#include "image.hpp"

#include "../input/input.hpp"

enum Scaling { SCALE_FULL, SCALE_VERTICAL, SCALE_HORIZONTAL, NOSCALING };

// step1 displays the dialog on the screen. The dialog won't be cleared before
// step2 has been called. Useful when one doesn't know how long the dialog
// should be displayed
void print_dialog_step1(const std::list<std::string>& messages, Overlay &o);
void print_dialog_step2(Overlay &o);
// waits for keypress
void print_dialog(const std::list<std::string>& messages);
// this is a wrapper for print_dialog_time_wait. time in ms
void print_dialog_wait(const std::list<std::string>& messages, int time);
// time in ms
void print_dialog_time_wait(const std::list<std::string>& messages, Overlay& overlay, int time = 500);
void print_dialog_fixed_size(const std::string& message, int max_horizontal_length, Overlay& dialog);

void cleanup_progressbar();
void print_progressbar();
int check_progressbar();

#endif
