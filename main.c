
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

// https://stackoverflow.com/questions/2100331/c-macro-definition-to-determine-big-endian-or-little-endian-machine
#define IS_BIG_ENDIAN (!*(unsigned char *)&(uint16_t){1})

static volatile long data[] = {
  0x0102030405060708,
  0,
  0x0102030405060708
};


int main(int argc, char** argv) {

  printf("data = %x %x %x\n", data[0], data[1], data[2]);
  printf("locations = %x %x %x\n", &data[0], &data[1], &data[2]);

  // Increment data[1] by 1
  data[1] += 1;

  // Read in this binary
  FILE* f = fopen("/proc/self/exe", "rb");
  fseek(f, 0, SEEK_END);
  long fsize = ftell(f);
  fseek(f, 0, SEEK_SET);  /* same as rewind(f); */

  char* binary = malloc(fsize + 1);
  fread(binary, 1, fsize, f);
  fclose(f);

  // record original FS file
  char f_path[2048];
  readlink("/proc/self/exe", f_path, 2047);

  // Record permissions as well
  struct stat st;
  stat(f_path, &st);

  // find location of data[0] in binary
  for (int i=0; i < fsize - sizeof(long); i++) {
#ifdef IS_BIG_ENDIAN
    if (binary[i] == 0x08 && binary[i+1] == 0x07 && binary[i+2] == 0x06 && binary[i+3] == 0x05 && binary[i+4] == 0x04 && binary[i+5] == 0x03 && binary[i+6] == 0x02 && binary[i+7] == 0x01) {
#else
    if (binary[i] == 0x01 && binary[i+1] == 0x02 && binary[i+2] == 0x03 && binary[i+3] == 0x04 && binary[i+4] == 0x05 && binary[i+5] == 0x06 && binary[i+6] == 0x07 && binary[i+7] == 0x08) {
#endif
      // Write data[1] to i+8 
      printf("Found 0x0102030405 at %d (fsize=%d)\n", i, fsize);
      
      char* data1_bytes = (long*) &data[1];

      binary[i+8] = data1_bytes[0];
      binary[i+9] = data1_bytes[1];
      binary[i+10] = data1_bytes[2];
      binary[i+11] = data1_bytes[3];
      binary[i+12] = data1_bytes[4];
      binary[i+13] = data1_bytes[5];
      binary[i+14] = data1_bytes[6];
      binary[i+15] = data1_bytes[7];

    }
  }

  // delete binary in FS
  remove(f_path);

  // write data[1] to binary
  f = fopen(f_path, "wb");
  fwrite(binary, fsize, 1, f);
  fclose(f);

  // Apply old permissions
  chmod(f_path, st.st_mode);

  // free memory
  free(binary);


  return 0;
}
