int add(int a, int b) {
  int result = a + b;
  return result;
}
int main(void) {
  int total = 0;
  for (int i = 0; i < 3; i++) {
    total = total + add(i, 1);
  }
  if (total > 0) {
    return total;
  } else {
    return 0;
  }
}