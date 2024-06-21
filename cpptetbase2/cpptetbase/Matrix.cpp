#include "Matrix.h"

int Matrix::nAlloc = 0;
int Matrix::nFree = 0;
//static함수로 해당 소스파일에서만 접근 가능. 
//클래스 레벨에서 정의된 정적 변수에 접근하기 때문에 모든 클래스 객체에서 동일한 값을 반환
int Matrix::get_nAlloc() { return nAlloc; }

int Matrix::get_nFree() { return nFree; }

int Matrix::get_dy() const { return dy; } 
//함수 뒤에 const가 붙는 경우, 해당 함수는 calling object의 멤버 변수(dy)를 수정하지 않음을 보장
int Matrix::get_dx() const { return dx; }

int **Matrix::get_array() const { return array; } //int **유의

void Matrix::alloc(int cy, int cx) {
  if ((cy <= 0) || (cx <= 0)) {
	dy = 0;
	dx = 0;
	array = NULL;
    nAlloc++;
    return;
  }
  dy = cy;
  dx = cx;
  array = new int*[dy];
  for (int y = 0; y < dy; y++)
    array[y] = new int[dx];
  for (int y = 0; y < dy; y++)
    for (int x = 0; x < dx; x++)
      array[y][x] = 0;
  
  nAlloc++;
}

Matrix::Matrix() { alloc(0, 0); }//디폴트 생성자 멤버 변수를 초기화 해주지 않으면 쓰레기 값이 들어감.

void Matrix::dealloc() { 
  if (array != NULL) {
    for (int y = 0; y < dy; y++)
      delete[] array[y]; //array[]을 삭제
    delete[] array;//array를 삭제.포인터 array는 소멸된 변수의 주소를 여전히 가지고 있음
    array = NULL; //dangling pointer인 array에 NULL 지정
  }

  nFree++;
}

Matrix::~Matrix() { dealloc(); }

Matrix::Matrix(int cy, int cx) {
  alloc(cy, cx);
  for (int y = 0; y < dy; y++)
    for (int x = 0; x < dx; x++)
      array[y][x] = 0;
}

Matrix::Matrix(const Matrix *obj) { // Matrix *oScreen = new Matrix(iScreen);
  alloc(obj->dy, obj->dx); //iScreen과 같은 크기의 Matrix 생성(요소는 0)
  for (int y = 0; y < dy; y++)
    for (int x = 0; x < dx; x++)
      array[y][x] = obj->array[y][x]; //oScreen의 array에 값 추가
}

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
//함수의 반환형이 Matrix *이므로 clip함수가 생성한 새로운 Matrix객체의 포인터를 반환
Matrix *Matrix::clip(int top, int left, int bottom, int right) { 
  int cy = bottom - top;
  int cx = right - left;
  Matrix *temp = new Matrix(cy, cx);
  for (int y = 0; y < cy; y++) { //cy,cx 유의
    for (int x = 0; x < cx; x++) {
      if ((top + y >= 0) && (left + x >= 0) &&
	      (top + y < dy) && (left + x < dx))
      	temp->array[y][x] = array[top + y][left + x];
      else {
      	cerr << "invalid matrix range";
        delete temp;
      	return NULL; //return이 포인터 변수이므로 NULL
      }
    }
  }
  return temp;
}
//Matrix 객체 그 자체를 반환. 메모리 공간이 동적으로 할당되지 않음
Matrix Matrix::clip_(int top, int left, int bottom, int right) { 
  int cy = bottom - top; 
  int cx = right - left; 
  Matrix temp(cy, cx);
  for (int y = 0; y < cy; y++) {
    for (int x = 0; x < cx; x++) {
      if ((top + y >= 0) && (left + x >= 0) &&
	      (top + y < dy) && (left + x < dx))
      	temp.array[y][x] = array[top + y][left + x]; //calling object의 array
      else {
      	cerr << "invalid matrix range";
      	return Matrix(); //defalut객체 반환
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
      }
    }
}

void Matrix::paste(const Matrix &obj, int top, int left) {
  for (int y = 0; y < obj.dy; y++)
    for (int x = 0; x < obj.dx; x++) {
      if ((top + y >= 0) && (left + x >= 0) &&
	      (top + y < dy) && (left + x < dx))
	      array[y + top][x + left] = obj.array[y][x];
      else {
	      cerr << "invalid matrix range";
      }
    }
}

Matrix *Matrix::add(const Matrix *obj) {
  if ((dx != obj->dx) || (dy != obj->dy)) return NULL; //return형이 포인터이므로 NULL
  Matrix *temp = new Matrix(dy, dx);
  for (int y = 0; y < dy; y++)
    for (int x = 0; x < dx; x++)
      temp->array[y][x] = array[y][x] + obj->array[y][x];
  return temp;
}
//반환되는 객체를 수정하지 않겠다는 뜻.
//&기호가 Matrix에 붙음
const Matrix operator+(const Matrix& m1, const Matrix& m2) { // friend function version of operator+ overloading 
  if ((m1.dx != m2.dx) || (m1.dy != m2.dy)) return Matrix();
  Matrix temp(m1.dy, m1.dx);
  for (int y = 0; y < m1.dy; y++)
    for (int x = 0; x < m1.dx; x++)
      temp.array[y][x] = m1.array[y][x] + m2.array[y][x];
  return temp;  
}

/*const Matrix Matrix::operator+(const Matrix& m2) const  { // member function version of operator+ overloading 함수 뒤에 붙은 const는 해당 함수가 calling object의 상태를 변경하지 않음을 보장 const Matrix& m2에서 const는 m2객체도 함수 내에서 변경되지 않음을 보장
  if ((dx != m2.dx) || (dy != m2.dy)) return Matrix();
  Matrix temp(dy, dx);
  for (int y = 0; y < dy; y++)
    for (int x = 0; x < dx; x++)
     temp.array[y][x] = array[y][x] + m2.array[y][x];
  return temp;  
}*/

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
  return out; //cout을 리턴
}

//리턴형에 &이 있으면 calling object의 레퍼런스 즉 현재 객체 자체를 반환한다고 생각하면 됨.
Matrix& Matrix::operator=(const Matrix& obj)
{//this는 calling object의 객체의 주소를 나타내는 포인터.
//*this는 calling object를 가리키는 포인터
  if (this == &obj) return *this;
  if ((dx != obj.dx) || (dy != obj.dy)) {
    if (array != NULL) dealloc();//기존의 array삭제
    alloc(obj.dy, obj.dx);
  }
  //크기를 같게 만들음
  for (int y = 0; y < dy; y++)
    for (int x = 0; x < dx; x++)
      array[y][x] = obj.array[y][x];
  return *this; // A = B이면 *this는 A(calling object)의 레퍼런스를 반환(현재 객체를 반환하여 연속적인 연산이 가능하도록 함)
}
/*shallow copy는 복사된 객체와 원본 객체가 같은 메모리 영역을 공유
deep copy는 복사된 객체는 원본 객체와 동일한 값을 가지지만, 별도의 메모리 영역을 가리킴.
객체가 포인터 멤버를 가지고 있거나 동적으로 할당된 메모리를 사용하는 경우,shallow copy를 사용하면 예상치 못한 결과가 발생가능*/