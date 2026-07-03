using Constructor = void (*)();
extern "C" {
Constructor __init_array_start;
Constructor __init_array_end;
void init_global_objects() {
  Constructor *C = &__init_array_start;
  while (C < &__init_array_end) {
    (*C)();
    C++;
  }
}
}
