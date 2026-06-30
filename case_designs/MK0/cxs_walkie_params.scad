// CXS Walkie Enclosure Parameter Library
// Units: millimeters

// ---------- Identity ----------
device_serial = "CXS-1";
case_revision = "MK0";

// ---------- Case ----------
case_h = 125;
case_w = 65;
case_d = 38;

wall = 2.4;
corner_r = 11;
front_shell_d = 18;
back_shell_d = case_d - front_shell_d;

// ---------- Clearances ----------
fit_clearance = 0.6;
loose_clearance = 1.0;
button_clearance = 0.4;

// ---------- Heltec V4 ----------
heltec_w = 25.3;
heltec_l = 52.5;
heltec_h = 10.7;

heltec_pocket_w = 26.0;
heltec_pocket_l = 53.2;
heltec_pocket_h = 11.5;

usb_c_w = 9.0;
usb_c_h = 3.5;

oled_window_w = 33.5;
oled_window_h = 19.5;

// ---------- Speaker ----------
speaker_w = 15.6;
speaker_l = 24.5;
speaker_h = 4.2;

speaker_grille_slot_w = 1.6;
speaker_grille_slot_h = 20;
speaker_grille_slot_gap = 2.6;
speaker_grille_slots = 6;

// ---------- Battery ----------
battery_d = 20.0;
battery_l = 70.0;
battery_cradle_wall = 1.8;

// ---------- Buttons ----------
ptt_hole_d = 24.4;
ptt_depth = 28;

channel_hole_d = 15.1;
channel_depth = 17;

power_hole_d = 12.4;
power_depth = 16;

// ---------- Mic ----------
mic_w = 17.4;
mic_l = 13.4;
mic_hole_spacing = 9.5;
mic_sound_hole_d = 2.0;

// ---------- Amp ----------
amp_w = 18.5;
amp_l = 20.0;
amp_terminal_h = 11;

// ---------- Layout Positions ----------
// Origin idea: case center X/Y, bottom = 0

speaker_y = 108;
oled_y = 86;
ptt_y = 72;
mic_y = 50;

battery_y = 45;
heltec_y = 82;
amp_y = 63;

// X placement
center_x = 0;
left_side_x = -case_w / 2;
right_side_x = case_w / 2;

// ---------- Fasteners ----------
insert_d = 4.2;
insert_h = 5.0;
screw_clearance_d = 3.2;

corner_screw_margin_x = 8;
corner_screw_margin_y = 10;

// ---------- Text / Branding ----------
label_depth = 0.5;
label_size = 5;