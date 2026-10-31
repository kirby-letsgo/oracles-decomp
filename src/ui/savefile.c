#include "ui/savefile.h"
#include <string.h>

// code/fileManagement.s: each file is a copy of wFileStart ($c5b0), $550 bytes, at SRAM
// $a010/$a560/$aab0 with backups at $b000/$b550/$baa0. Offsets below are from wFileStart.
#define FILE_SIZE UI_FILE_SIZE
#define OFS_STRING 0x02
#define OFS_NAME 0x52
#define OFS_LINKED 0x62
#define OFS_HERO 0x63
#define OFS_COMPLETED 0x64

static const uint16_t copy1[UI_FILES] = {0x0010, 0x0560, 0x0ab0};
static const uint16_t copy2[UI_FILES] = {0x1000, 0x1550, 0x1aa0};

uint16_t ui_file_checksum(const uint8_t *file) {
  uint16_t sum = 0;
  for (int i = 2; i < FILE_SIZE; i += 2) sum = (uint16_t)(sum + (file[i] | (file[i + 1] << 8)));
  return sum;
}

// A name character as the menus can draw it: ASCII as it is, the accented letters the European
// releases' name entry offers (the text encoding's $80-$d8, tools/build/parseText.py in the
// disassembly) as their base letter, anything else '?' ('.' in the table, which spells no '?' so
// that no "??x" trigraph can form).
static char name_char(uint8_t ch) {
  static const char accented[] =
      "AAAACEEEEIINOOUUU"            // $80-$90  À Â Ä Æ Ç È É Ê Ë Î Ï Ñ Ö Œ Ù Û Ü
      "s......!."                    // $91-$99  ß, ¡ at $98 (¿ at $99 stays '?')
      "......"                       // $9a-$9f
      "aaaaceeeeiinoouuu"            // $a0-$b0  à â ä æ ç è é ê ë î ï ñ ö œ ù û ü
      "'.............."              // $b1-$bf  ´
      "OoAIOUIOA......"              // $c0-$ce  Ô ° Á Í Ó Ú Ì Ò Å
      "."                            // $cf
      "oaaiouioa";                   // $d0-$d8  ô ª á í ó ú ì ò å
  if (ch >= 0x20 && ch < 0x80) return (char)ch;
  if (ch >= 0x80 && ch <= 0xd8 && accented[ch - 0x80] != '.') return accented[ch - 0x80];
  return '?';
}

static bool verify(const uint8_t *file, bool ages) {
  uint16_t stored = (uint16_t)(file[0] | (file[1] << 8));
  return stored == ui_file_checksum(file) && memcmp(file + OFS_STRING, ages ? "Z21216-0" : "Z11216-0", 8) == 0;
}

const uint8_t *ui_file_data(const uint8_t *sram, size_t size, bool ages, int slot) {
  if (!sram || slot < 0 || slot >= UI_FILES) return NULL;
  if (size >= (size_t)copy1[slot] + FILE_SIZE && verify(sram + copy1[slot], ages)) return sram + copy1[slot];
  if (size >= (size_t)copy2[slot] + FILE_SIZE && verify(sram + copy2[slot], ages)) return sram + copy2[slot];
  return NULL;
}

int ui_read_files(const uint8_t *sram, size_t size, bool ages, UiFileInfo out[UI_FILES]) {
  int ofs_health = ages ? 0xfa : 0xf2, ofs_essences = ages ? 0x10f : 0x10b;
  int n = 0;
  for (int i = 0; i < UI_FILES; i++) {
    UiFileInfo *f = &out[i];
    memset(f, 0, sizeof *f);
    const uint8_t *file = ui_file_data(sram, size, ages, i);
    if (!file) continue;
    f->valid = true;
    for (int k = 0; k < 5 && file[OFS_NAME + k]; k++) f->name[k] = name_char(file[OFS_NAME + k]);
    f->health = file[ofs_health];
    f->max_health = file[ofs_health + 1];
    f->essences = file[ofs_essences];
    for (int b = 0; b < 8; b++) f->essence_count += (f->essences >> b) & 1;
    f->linked = file[OFS_LINKED] != 0;
    f->hero = file[OFS_HERO] != 0;
    f->completed = file[OFS_COMPLETED] != 0;
    n++;
  }
  return n;
}
