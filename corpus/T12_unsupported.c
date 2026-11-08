int main(void) {
  switch (x) {
  case 1:
    return 1;
  default:
    goto done;
  }
done:
  return 0;
}