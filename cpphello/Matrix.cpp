#include "Matrix.h"

void Matrix::alloc(int cy, int cx) {
  if ((cy < 0) || (cx < 0)) return;
  dy = cy;
  dx = cx;
  array = new int*[dy]; 
  //크기가 dy인 포인터 배열을 의미, 이 배열은 각각의 포인터가 int배열을 가리키도록 할당
  //포인터 배열을 사용하여 2차원 배열을 동적으로 할당하는 이유는, 2차원 배열의 각 행이 독립적인 메모리 블록으로 할당되어야 하기 때문
  //array는 "int**"형식의 포인터 변수, 각 포인터가 int배열을 가리키는 2차원 배열의 첫번째 행을 가리킨다.
  for (int y = 0; y < dy; y++)
    array[y] = new int[dx]; //array[][3]를 만드는 과정
  for (int y = 0; y < dy; y++)
    for (int x = 0; x < dx; x++)
      array[y][x] = 0; //array[][] 0으로 초기화
}

int Matrix::get_dy() { return dy; }

int Matrix::get_dx() { return dx; }

int **Matrix::get_array() { return array; }

Matrix::Matrix() { alloc(0, 0); }

Matrix::~Matrix() { 
  for (int y = 0; y < dy; y++)
    delete array[y];
  delete array;
}
//일반 생성자
Matrix::Matrix(int cy, int cx) {
  alloc(cy, cx);
  for (int y = 0; y < dy; y++)
    for (int x = 0; x < dx; x++)
      array[y][x] = 0;
}
//복사 생성자 1
Matrix::Matrix(const Matrix *obj) {
  alloc(obj->dy, obj->dx);
  for (int y = 0; y < dy; y++)
    for (int x = 0; x < dx; x++)
      array[y][x] = obj->array[y][x];
}
//복사 생성자 2
Matrix::Matrix(const Matrix &obj) {
  alloc(obj.dy, obj.dx);
  for (int y = 0; y < dy; y++)
    for (int x = 0; x < dx; x++)
      array[y][x] = obj.array[y][x];
}

Matrix::Matrix(int *arr, int col, int row) {
  alloc(col, row);
  for (int y = 0; y < dy; y++)
    for (int x = 0; x < dx; x++)
      array[y][x] = arr[y * dx + x];
}

Matrix *Matrix::clip(int top, int left, int bottom, int right) {
  int cy = bottom - top;
  int cx = right - left;
  Matrix *temp = new Matrix(cy, cx);
  for (int y = 0; y < cy; y++) {
    for (int x = 0; x < cx; x++) {
      if ((top + y >= 0) && (left + x >= 0) &&
	  (top + y < dy) && (left + x < dx))
	temp->array[y][x] = array[top + y][left + x];
      else {
	cerr << "invalid matrix range";
	return NULL;
      }
    }
  }
  return temp;
}

void Matrix::paste(const Matrix *obj, int top, int left) {
  for (int y = 0; y < obj->dy; y++)
    for (int x = 0; x < obj->dx; x++) {
      if ((top + y >= 0) && (left + x >= 0) &&
	  (top + y < dy) && (left + x < dx))
	array[y + top][x + left] = obj->array[y][x];
      else {
	cerr << "invalid matrix range";
	return NULL;
      }
    }
}

Matrix *Matrix::add(const Matrix *obj) {
  if ((dx != obj->dx) || (dy != obj->dy)) return NULL;
  Matrix *temp = new Matrix(dy, dx);
  for (int y = 0; y < dy; y++)
    for (int x = 0; x < dx; x++)
      temp->array[y][x] = array[y][x] + obj->array[y][x];
  return temp;
}

int Matrix::sum() {
  int total = 0;
  for (int y = 0; y < dy; y++)
    for (int x = 0; x < dx; x++)
      total += array[y][x];
  return total;
}

void Matrix::mulc(int coef) {
  for (int y = 0; y < dy; y++)
    for (int x = 0; x < dx; x++)
      array[y][x] = coef * array[y][x];
}

Matrix *Matrix::int2bool() {
  Matrix *temp = new Matrix(dy, dx);
  int **t_array = temp->get_array();
  for (int y = 0; y < dy; y++)
    for (int x = 0; x < dx; x++)
      t_array[y][x] = (array[y][x] != 0 ? 1 : 0);
  
  return temp;
}

bool Matrix::anyGreaterThan(int val) {
  for (int y = 0; y < dy; y++) {
    for (int x = 0; x < dx; x++) {
      if (array[y][x] > val)
	return true;
    }
  }
  return false;
}

void Matrix::print() {
  cout << "Matrix(" << dy << "," << dx << ")" << endl;
  for (int y = 0; y < dy; y++) {
    for (int x = 0; x < dx; x++)
      cout << array[y][x] << " ";
    cout << endl;
  }
}


ostream& operator<<(ostream& out, const Matrix& obj){
  out << "Matrix(" << obj.dy << "," << obj.dx << ")" << endl;
  for(int y = 0; y < obj.dy; y++){
    for(int x = 0; x < obj.dx; x++)
      out << obj.array[y][x] << " ";
    out << endl;
  }
  out << endl;
  return out;
}

Matrix& Matrix::operator=(const Matrix& obj)
{//this포인터는 calling object의 주소값을 저장
//*this포인터는 calling object를 가리킴
  if (this == &obj) return *this;
  if ((dx != obj.dx) || (dy != obj.dy))
    alloc(obj.dy, obj.dx);

  for (int y = 0; y < dy; y++)
    for (int x = 0; x < dx; x++)
      array[y][x] = obj.array[y][x];
  return *this;
}
