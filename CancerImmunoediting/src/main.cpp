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
#include <cstdlib>

// ターミナル画面制御用
#define ESC             "\033["
/*----------------------------------------------------------------------------
 *  強調表示
 *----------------------------------------------------------------------------*/
#define STANDARD        ESC"0m"
#define BOLD            ESC"1m"
#define UNDERLINE       ESC"4m"
#define REVERSE         ESC"7m"
/*----------------------------------------------------------------------------
 *  文字色
 *----------------------------------------------------------------------------*/
#define BLACK           ESC"30m"
#define RED             ESC"31m"
#define GREEN           ESC"32m"
#define YELLOW          ESC"33m"
#define BLUE            ESC"34m"
#define MAGENTA         ESC"35m"
#define CYAN            ESC"36m"
#define WHITE           ESC"37m"
/*-----------------------------------------------------------------------------
 *  クリア
 *-----------------------------------------------------------------------------*/
#define CLEAR_RIGHT     ESC"0K"
#define CLR_BG          ESC"49m"
#define CLR_ST          ESC"39m"

///////////////////////////////////////////////////////////
// 汎用マクロ
#define FOR(i, n)               for(int (i)=0; (i)<(n); (i)++) // i: 0 ~ (n-1)
#define REP(i, min, max)        for(int (i)=(min); (i)<=(max); (i)++)

#define ECHO(x)                 do { std::cout<< CLEAR_RIGHT << "----> " \
                                <<GREEN<<BOLD<<x<<STANDARD<<CLR_ST<<"" \
                                <<std::endl; }while(0);
#define DEBUG(x)                do { std::cerr<<BOLD<<"[ DEBUG ] " \
                                <<CLR_ST<<STANDARD<<#x<<" <-- "<<BOLD<<(x) \
                                <<STANDARD<<" (L"<<__LINE__<<")" \
                                <<""<<__FILE__<<std::endl; }while(0);
#define POINT                   do { static int point = 0; std::cerr \
                                <<BOLD<<RED<<"[ POINT ] "<<CLR_ST \
                                <<STANDARD<<"(L"<<__LINE__<<")" \
                                <<" "<<__FILE__<<" - "<<point++ \
                                <<std::endl; }while(0);

#define VECTOR(type)            std::vector< type >
#define ITERATOR(type)          std::vector< type >::iterator
#define EACH(i,c)               for(typeof((c).begin()) i=(c).begin(); i!=(c).end(); ++i)

#define SAFE_DELETE(p)          delete p; p = NULL;
#define SAFE_DELETE_ARRAY(p)    delete[] p; p = NULL;

#define SEPARATOR               " "

// 定数パラメータの定義する。

// ランドスケープの幅と高さを設定する。
const int WIDTH = 30;
const int HEIGHT = 30;

// 最大計算期間を設定する。
const int STEP = 100;

// 細胞数を設定する。
const int CELL_SIZE = 100;

// クラスを定義していく。

// 乱数生成用のクラスを作成する。
// シングルトンパターンを使用する。
class Random {
public:
  static Random& Instance() { static Random singleton; return singleton; }
  int randomInt() { return rand(); }
  double randomDouble() { return ((double)rand()+1.0)/((double)RAND_MAX+2.0); }
  int uniformInt(int min, int max) {
    int ret = randomInt()%( max - min + 1 ) + min;
    return ret;
  }
  double uniformDouble( double min, double max ) {
    return uniformInt(min, max-1) + randomDouble();
  }
  bool probability( double prob ) {
    if( prob > uniformDouble( 0, 100 ) ) { return true; }
    else { return false; }
  }
  bool randomBool() { return probability(50) ? true : false; }
  int randomSign() { return probability(50) ? -1 : 1; }
private:
  Random() { srand((unsigned)time(NULL)); }
};

// 細胞土地のインターフェイスを作成する。
// 幅と高さを持つ。
class __Landscape {
 public:
   __Landscape() : width_(WIDTH), height_(HEIGHT) { }
  int width() const { return width_; }
  int height() const { return height_; }

  // ランドスケープ上に存在する点かどうかを評価する。
  bool isExistingPoint(int x, int y) {
    if( x < 0 ) return false;
    if( y < 0 ) return false;
    if( x > WIDTH-1 ) return false;
    if( y > HEIGHT-1 ) return false;
    return true;
  }
 private:
  int width_, height_;
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

/*
 * モデル上に存在するためには、座標が必要になるので、
 * 座標を持つエージェントのためのインターフェイスを作成する。
 */
class __Location {
 public:
  int x() const { return x_; }
  int y() const { return y_; }
  void setX(int x) { x_ = x; }
  void setY(int y) { y_ = y; }

  // スケープ上にランダムに配置する。
  void randomSet() {
    setX(Random::Instance().uniformInt(0, WIDTH-1)); 
    setY(Random::Instance().uniformInt(0, HEIGHT-1));
  }
 private:
  int x_, y_;
};

// 移動するエージェントのインターフェイスを作成する。
class __Mobile : public __Location {
 public:
  void move() { }
  void move(int tox, int toy);
  // ランドスケープ上を移動させる。
  // 壁あり。
  virtual void move( __Landscape& landscape ) {
    Random& random = Random::Instance();
    int to_x = x(); int to_y = y();
    if( random.randomBool() ) { to_x += random.randomSign(); }
    if( random.randomBool() ) { to_y += random.randomSign(); }
    if( landscape.isExistingPoint( to_x, to_y ) ) {
      setX( to_x ); setY( to_y );
    }
  }
  int movementDistance() const { return movement_distance_; }
  private:
    int movement_distance_;
};

// 酸化的リン酸化を利用してエネルギーを産生するクラスを作成する。
// グルコースと酸素に依存する。

// 嫌気的解糖系を利用してエネルギーを産生するクラスを作成する。
// グルコースのみに依存する。

// 細胞のインターフェイスを作成する。
// 細胞は代謝する。
// エネルギーを持つ。
class __Cell : public __Mobile {
  public:
  void metabolize(__SugerScape& landscape) {
  }

  virtual void move( __Landscape& landscape ) {
    Random& random = Random::Instance();
    int to_x = x(); int to_y = y();
    if( random.randomBool() ) { to_x += random.randomSign(); }
    if( random.randomBool() ) { to_y += random.randomSign(); }
    if( landscape.isExistingPoint( to_x, to_y ) ) {
      setX( to_x ); setY( to_y );
    }
  }
private:
  double energy_;
};

// 正常細胞のクラスを作成する。
// 移動する。細胞スケープにおいて、同じ位置に存在できる。
// 移動はしない。分裂はする。？？
class NormalCell : public __Cell {
 public:
 private:
};

// 時間を更新するクラスを作成する。
// どこからアクセスしても同じ時間になるために、
// シングルトンパターンを利用する。
class StepKeeper {
 public:
  static StepKeeper& Instance() { 
    static StepKeeper singleton; return singleton;
    }
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

// 出力用の関数を作成する。

// ステップ数と一緒、そのときの値を出力する関数
template < typename T >
void output_value_with_term( const char *fname, T value ) {
  int step = StepKeeper::Instance().step();
  std::ofstream ofs(fname, std::ios_base::out | std::ios_base::app);
  ofs << step << SEPARATOR;
  ofs << value << std::endl;
};

/*
 * 細胞クラスの分布を出力する関数
 */
void output_cell_map( VECTOR(__Cell *)& cells ) {
  // ファイル名
  char file_name[256];
  sprintf(file_name, "%d-cell.txt", StepKeeper::Instance().step());
  std::ofstream cell_map_ofs(file_name);
  int location_map[HEIGHT][WIDTH] = {};
  EACH(it_cell, cells) {
    __Cell& cell = **it_cell;
    location_map[cell.y()][cell.x()]++;
  }
  FOR(i, HEIGHT) {
    FOR(j, WIDTH) {
      cell_map_ofs << i << SEPARATOR;
      cell_map_ofs << j << SEPARATOR;
      cell_map_ofs << location_map[i][j];
      cell_map_ofs << std::endl;
    }
    cell_map_ofs << std::endl;
  }
}

// エントリーポイント
int main() {
  ECHO("Cancer Immunoediting Model");

  // 期間クラスのインスタンスを生成する
  StepKeeper &stepKeeper = StepKeeper::Instance();
  stepKeeper.setMaxStep( STEP );

  // グルコーススケープのインスタンスを作成する。  
  GlucoseScape *gs = new GlucoseScape();
  
  __Landscape *landscape = new __Landscape();
  // 細胞を初期化していく。
  // TODO: 普通の細胞は細胞土地のほうがいいかも
  VECTOR(__Cell *) cells;
  FOR(i, CELL_SIZE) {
    __Cell *nm = new __Cell();
    nm->randomSet();
    cells.push_back( nm );
  }

  // 計算を実行する ---------------------------------------
  while( stepKeeper.loop() ) {
    // 細胞が移動する。
    EACH( it_cell, cells ) {
      __Cell& cell = **it_cell;
      cell.move( *landscape );
    }

    // 細胞の分布を出力する
    output_cell_map( cells );
  }
  // ------------------------------------------------------

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

  return 0;
}
