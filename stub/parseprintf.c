struct printformat {
  char buf[sizeof(" #+-0" "*.*x")];
  int pad, prec;
}
