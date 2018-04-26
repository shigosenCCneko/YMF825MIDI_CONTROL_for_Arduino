#define MIDI_GENELAL_COMMAND 0x10
#define MIDI_SYSEX_START     0x20
#define MIDI_SYSEX_DATA      0x30
#define MIDI_SYSEX_END       0x40
#define MIDI_NOTE_OFF        0x80
#define MIDI_NOTE_ON         0x90
#define MIDI_CONTROL_CHANGE  0xb0
#define MIDI_PROGRAM_CHANGE  0xc0
#define MIDI_PITCH_BEND      0xe0


void SetupHardware(void);
void midi_command(uint8_t,uint8_t,uint8_t,uint8_t);
void midi_sysEx(uint8_t *,uint8_t);
void change_pitchbend(char,char,char);
void change_expression(char,char);
void change_modulation(char,char);
void change_part_level(char,char);
void hold_on(char);
void hold_off(char);
void write_burst(void);
void flush_spi_buff(void);
void reset_ymf825(void);
void startup_sound(void);


