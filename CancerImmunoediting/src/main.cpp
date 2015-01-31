//
// - 正常細胞とがん細胞の、糖代謝の違いを表現するモデル
//
// - エネルギーの酸性には、酸化的リン酸化と嫌気的解糖系の２種類ある。
// - がん細胞では、酸化的リン酸化の利用が低下し、
//   嫌気的解糖系の利用が増加する。（ワールブルク効果）
//
// Author Naoki Ueda
//
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

// 汎用マクロ
#define FOR(i, n)           for(int (i)=0; (i)<(n); (i)++) // i: 0 ~ (n-1)
#define REP(i, min, max)    for(int (i)=(min); (i)<=(max); (i)++)

// #define ECHO(x)             do { std::cout<< CLEAR_RIGHT << "----> "<<GREEN<<BOLD<<x<<STANDARD<<CLR_ST<<""<<std::endl; }while(0);
#define ECHO(x)             do { std::cout << "---> "<<x<<""<<std::endl; }while(0);
// #define POINT               do { static int point = 0; std::cerr<<BOLD<<RED<<"[ POINT ] "<<CLR_ST<<STANDARD<<"(L"<<__LINE__<<")"<<" "<<__FILE__<<" - "<<point++<<std::endl; }while(0);

#define VECTOR(type)        std::vector< type >
#define ITERATOR(type)      std::vector< type >::iterator
#define EACH(i,c)           for(typeof((c).begin()) i=(c).begin(); i!=(c).end(); ++i)

#define SAFE_DELETE(p)          delete p; p = NULL;
#define SAFE_DELETE_ARRAY(p)    delete[] p; p = NULL;

#define SEPARATOR " "

// 定数パラメータの定義する。

// ランドスケープの幅と高さを設定する。
const int WIDTH = 50;
const int HEIGHT = 50;
// 最大計算期間を設定する。
const int STEP = 100;

// 細胞数を設定する。
const int CELL_SIZE = 100;

// クラスを定義していく。

// 細胞土地のインターフェイスを作成する。
// 幅と高さを持つ。
class __Landscape {
 public:
  int width() const { return width_; }
  int height() const { return height_; }
 private:
  int width_, height_;
};

// モデル上に存在するためには、座標が必要になるので、
// 座標を持つエージェントのためのインターフェイスを作成する。
class __Location {
 public:
  int x() const { return x_; }
  int y() const { return y_; }
  void setX(int x) { x_ = x; }
  void setY(int y) { y_ = y; }
  void randomSet() { setX(0); setY(0); }
 private:
  int x_, y_;
};

// 移動するエージェントのインターフェイスを作成する。
class __Mobile : public __Location {
 public:
  void move();
  void move(int tox, int toy);
 private:
};

// 酸化的リン酸化を利用してエネルギーを産生するクラスを作成する。
// グルコースと酸素に依存する。

// 嫌気的解糖系を利用してエネルギーを産生するクラスを作成する。
// グルコースのみに依存する。

// 細胞のインターフェイスを作成する。
// 細胞は代謝する。
// エネルギーを持つ。
class Cell {
public:
  void metabolize(double glucose, double oxygen);
private:
  double energy_;
};

// 正常細胞のクラスを作成する。
// 移動する。細胞スケープにおいて、同じ位置に存在できる。
// 移動はしない。分裂はする。？？
class NormalCell : public __Location {
 public:
 private:
};

// シュガースケープのクラスを作成する。
// シュガーを生産できる。
class __SugerScape : public __Landscape {
 public:
  void generate();
 private:
};

// グルコースのクラスを作成する。
class GlucoseScape : public __SugerScape {
 public:
  GlucoseScape() {
    FOR(i, HEIGHT) {
      FOR(j, WIDTH) {
        glucose_map_[i][j] = i+j;
      }
    }
  }
  int glucose(int x, int y) const { return glucose_map_[x][y]; }
 private:
  int glucose_map_[HEIGHT][WIDTH];
};

// 酸素のクラスを作成する。
class OxygenScape : public __SugerScape {
 public:
  int oxygen(int x, int y) const;
 private:
};

// 時間を更新するクラスを作成する。
// どこからアクセスしても同じ時間になるために、
// シングルトンパターンを利用する。
class StepKeeper {
 public:
  static StepKeeper& Instance() { static StepKeeper singleton; return singleton; }
  void proceed() { step_++; }
  int step() const { return step_; }
  int maxStep() const { return max_step_; }
  void setMaxStep( int maxstep ) { max_step_ = maxstep; }
  bool loop() {
    proceed();
    if( step() <= maxStep() ) return true;
    else return false;
  }
 private:
  StepKeeper() : step_(0), max_step_(0) { }
  int step_;
  int max_step_;
};

// メインルーチン
int main() {
  ECHO("Cancer Immunoediting Model");

  // 期間クラスのインスタンスを生成する
  StepKeeper &stepKeeper = StepKeeper::Instance();
  stepKeeper.setMaxStep( STEP );

  GlucoseScape *gs = new GlucoseScape();

  // 細胞を初期化していく。
  // TODO: 普通の細胞は細胞土地のほうがいいかも
  VECTOR(NormalCell *) normal_cells;
  FOR(i, CELL_SIZE) {
    NormalCell *nm = new NormalCell();
    nm->randomSet();
    normal_cells.push_back( nm );
  }
  std::ofstream cell_map_ofs("cell.txt");
  FOR(i, CELL_SIZE) {
  }

  // 現在のグルコースの分布を出力する。
  std::ofstream glucose_map_ofs("test.txt");
  FOR(i, HEIGHT) {
    FOR(j, WIDTH) {
      glucose_map_ofs << i << SEPARATOR;
      glucose_map_ofs << j << SEPARATOR;
      glucose_map_ofs << gs->glucose(i, j);
      glucose_map_ofs << std::endl;
    }
    glucose_map_ofs << std::endl;
  }

  // 計算を実行する
  while( stepKeeper.loop() ) {
  }

  return 0;
}
