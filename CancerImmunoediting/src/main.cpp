/**
 * 正常細胞とがん細胞との、糖代謝の違いを表現するモデル。
 *
 * 細胞、グルコーススケープ、酸素スケープ
 *
 * TODO:
 *   - T細胞
 *
 * @author Naoki Ueda
 */
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
  <<GREEN<<x<<STANDARD<<CLR_ST<<"" \
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
#define FOREACH(i,c)            for(typeof((c).begin()) i=(c).begin(); i!=(c).end(); )

#define SAFE_DELETE(p)          delete p; p = NULL;
#define SAFE_DELETE_ARRAY(p)    delete[] p; p = NULL;

#define SEPARATOR               " "

/*
 * 定数パラメータを定義する。
 */

// ランドスケープの幅と高さを設定する。
const int WIDTH  = 30; //: 幅
const int HEIGHT = 30; //: 高さ

const int INITIAL_CELL_ENERGY = 10; //:

/* グルコース, 酸素の再生量 /1step */
const double GLUCOSE_GENERATE = 0.1; //:
const double OXYGEN_GENERATE = 0.1; //:

// 最大計算期間を設定する。
const int STEP = 1000; //:

// 細胞数を設定する。
const int CELL_SIZE = 100; //:

const double CELL_METABOLIZE_GLUCOSE = 2; //:

const double CELL_DEATH_THRESHOLD_ENERGY = 0; //:
const double CELL_DIVISION_THRESHOLD_ENERGY = 15; //:

/*
 * クラスを定義していく。
 */

/*
 * 乱数生成用のクラスを作成する。
 */

/**
 * @brief 乱数生成用のクラス
 *
 * シングルトンパターンを使用する。
 */
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
    ~Random() { }
};

/**
 * @brief ランドスケープのインターフェイス
 *
 * 幅と高さを持つ
 */
class __Landscape {
  public:
    __Landscape() : width_(WIDTH), height_(HEIGHT) { }
    ~__Landscape() { }
    
    int width() const { return width_; }
    int height() const { return height_; }

    // ランドスケープ上に存在する点かどうかを評価する。
    bool isExistingPoint( int x, int y );
  private:
    int width_, height_;
};

/**
 * @brief グルコースのクラス
 */
class GlucoseScape : public __SugarScape {
  public:
    GlucoseScape() {
      FOR(i, HEIGHT) {
        FOR(j, WIDTH) {
          glucose_map_[i][j] = 5;
        }
      }
    }
    virtual void generate() {
      FOR(i, WIDTH) {
        FOR(j, HEIGHT) {
          glucose_map_[i][j] += GLUCOSE_GENERATE;
        }
      }
    }
    double glucose(int x, int y) const { return glucose_map_[x][y]; }
    void setGlucose(int x, int y, double value) { glucose_map_[x][y] = value; }
  private:
    double glucose_map_[HEIGHT][WIDTH];
};

/**
 * @brief 酸素のクラスを作成する。
 */
class OxygenScape : public __SugarScape {
  public:
    OxygenScape() {
      FOR(i, HEIGHT) {
        FOR(j, WIDTH) {
          oxygen_map_[i][j] = 5;
        }
      }
    }
    double oxygen(int x, int y) const { return oxygen_map_[x][y]; }
    void setOxygen(int x, int y, double value) { oxygen_map_[x][y] = value; }
    virtual void generate() {
      FOR(i, WIDTH) {
        FOR(j, HEIGHT) {
          oxygen_map_[i][j] += OXYGEN_GENERATE;
        }
      }
    }
  private:
    double oxygen_map_[HEIGHT][WIDTH];
};

/**
 * @brief 位置情報のクラス
 * 
 * モデル上に存在するためには、座標が必要になるので、
 * 座標を持つエージェントのためのインターフェイスを作成する。
 */
class __Location {
  public:
    __Location() { }
    ~__Location() { }
    int x() const { return x_; }
    int y() const { return y_; }
    void setX(int x) { x_ = x; }
    void setY(int y) { y_ = y; }
    void setLocation(int x, int y) { setX(x); setY(y); }

    // スケープ上にランダムに配置する。
    void randomSetLocation() {
      setX(Random::Instance().uniformInt(0, WIDTH-1)); 
      setY(Random::Instance().uniformInt(0, HEIGHT-1));
    }
  private:
    int x_, y_;
};

/**
 * @brief 移動体のインターフェイス
 */
class __Mobile : public __Location {
  public:
    __Mobile() { }
    virtual ~__Mobile() { }
    void move() { }
    void move(int tox, int toy);
    // ランドスケープ上を移動させる。
    // 壁あり。


    /**
     * 移動する。
     * 
     * @param landscape スケープ
     * @return 移動した距離を、マンハッタン距離で返す。
     */
    virtual double move( __Landscape& landscape ) {
      Random& random = Random::Instance();
      double distance = 0;
      int from_x = x(); int from_y = y();
      int to_x = from_x; int to_y = from_y;
      if( random.randomBool() ) { to_x += random.randomSign(); }
      if( random.randomBool() ) { to_y += random.randomSign(); }

      if( landscape.isExistingPoint( to_x, to_y ) ) {
        setX( to_x ); setY( to_y );
        distance = abs(from_x-to_x) + abs(from_y-to_y);
      }
      return distance;
    }
    int movementDistance() const { return movement_distance_; }
  private:
    int movement_distance_;
};

/**
 * @brief 細胞クラス
 *
 * 代謝する。
 * エネルギーを持つ。
 */
class __CellState;
class NormalCellState;
class Cell : public __Mobile {
 public:
  Cell();
  virtual ~Cell() { }
  
  double energy() const { return energy_; }
  void setEnergy( double energy ) { energy_ = energy; }
  void consumeEnergy( double consume ) { setEnergy( energy() - consume ); }
  void gatherEnergy( double gather ) { setEnergy( energy() + gather ); }
  
  /** 代謝する */
  void metabolize( GlucoseScape& gs, OxygenScape& os );
  
  /** スケープ上を移動する */
  virtual double move( __Landscape& landscape ) {
    double distance = __Mobile::move(landscape);
    consumeEnergy( distance );
    return distance;
  }
 private:
  double energy_;
  __CellState *state_;
};

/*
 * 細胞の状態をあらわすクラスを作成する。
 * 正常細胞とがん細胞との、状態による違いをプログラムする。
 */

/**
 * @brief 細胞状態をあわらす抽象クラス
 *
 * Stateパターンを使用する。
 * シングルトンパターンを使用する。
 */
class __CellState {
public:
  // virtual __CellState& Instance() = 0;
  virtual void metabolize( Cell& cell, GlucoseScape& gs, OxygenScape& os ) = 0;
private:
};

/**
 * @brief 正常細胞の状態を表すクラス
 *
 * 酸化的リン酸化を利用してエネルギーを産生する。
 */
class NormalCellState : public __CellState {
public:
  static NormalCellState& Instance() {
    static NormalCellState singleton;
    return singleton;
  }

  /**
   * グルコースと酸素を利用してエネルギーを産生する。
   * 
   * @param cell 細胞
   * @param gs グルコーススケープ
   * @param os 酸素スケープ
   */
  virtual void metabolize( Cell& cell,  GlucoseScape& gs, OxygenScape& os ) {
    int g = gs.glucose(cell.x(), cell.y());
    int gathering = CELL_METABOLIZE_GLUCOSE;
    if( g >= gathering ) {
      cell.gatherEnergy( gathering );
      gs.setGlucose( cell.x(), cell.y(), g-gathering );
    }
  }
private:
  NormalCellState() { }
};

/**
 * @brief がん細胞状態を表すクラス
 *
 * 嫌気的解糖系を利用してエネルギーを産生する。
 */
class CancerCellState : public __CellState {
public:
  virtual void metabolize( Cell& cell, GlucoseScape& gs, OxygenScape& os );
private:
};
/**
 * @brief ステップ管理するクラス
 *
 * 時間を更新するクラスを作成する。
 * どこからアクセスしても同じ時間になるために、
 * シングルトンパターンを利用する。
 */
class StepKeeper {
  public:
    static StepKeeper& Instance();

    int step() const { return step_; }
    int maxStep() const { return max_step_; }
    void setMaxStep( int maxstep ) { max_step_ = maxstep; }

    /* ステップを進める */
    void proceed() { step_++; }

    /* 最大ステップまでループする */
    bool loop();

    /* 指定した間隔で真を返す。 */
    bool isInterval( int interval );

  private:
    StepKeeper() : step_(0), max_step_(0) { }
    int step_;
    int max_step_;
};

/*
 * 出力用の関数を作成する。
 */

/*
 * ステップ数と一緒に、そのときの値を出力する関数
 */
template < typename T >
void output_value_with_step( const char *fname, T value ) {
  int step = StepKeeper::Instance().step();
  std::ofstream ofs(fname, std::ios_base::out | std::ios_base::app);
  ofs << step << SEPARATOR;
  ofs << value << std::endl;
};

/*
 * ステップ数と一緒に、その時の値を出力する関数
 */
template < typename T >
void output_map_with_value( const char *fname,  VECTOR(T *)& agents ) {
  // ファイル名
  char file_name[256];
  sprintf(file_name, "%d-%s.txt", StepKeeper::Instance().step(), fname);
  std::ofstream agent_map_ofs(file_name);

  // マップの全てのいちを0で初期化する。
  int agent_map[HEIGHT][WIDTH] = {};
  EACH(it_agent, agents) {
    T& agent = **it_agent;
    agent_map[agent.y()][agent.x()]++;
  }
  FOR(i, HEIGHT) {
    FOR(j, WIDTH) {
      agent_map_ofs << i << SEPARATOR;
      agent_map_ofs << j << SEPARATOR;
      agent_map_ofs << agent_map[i][j];
      agent_map_ofs << std::endl;
    }
    agent_map_ofs << std::endl;
  }
}

/**
 * 細胞クラスの、スケープ上での2次元マップを出力する。
 * @param cells 細胞配列
 */
// void output_cell_map( VECTOR(Cell *)& cells );

// 細胞クラスの平均エネルギーを出力する。
void output_cell_energy_average( VECTOR(Cell *)& cells );

// 現在のシュガースケープの分布を出力する。
void output_glucose_map( GlucoseScape& gs );
void output_oxygen_map( OxygenScape& os );

// エントリーポイント
int main() {
  ECHO("Cancer Immunoediting Model");

  // 期間クラスのインスタンスを生成する
  StepKeeper &stepKeeper = StepKeeper::Instance();
  stepKeeper.setMaxStep( STEP );

  // グルコース、酸素マップのインスタンスを作成する。
  GlucoseScape *gs = new GlucoseScape();
  OxygenScape *os = new OxygenScape();

  // 細胞を初期化していく。
  // TODO: 普通の細胞は細胞土地のほうがいいかも
  VECTOR(Cell *) cells;
  FOR(i, CELL_SIZE) {
    Cell *nm = new Cell();
    nm->randomSetLocation();
    cells.push_back( nm );
  }

  // 計算を実行する ---------------------------------------
  while( stepKeeper.loop() )
  {
    /*
     * 細胞を移動させる。
     */
    EACH( it_cell, cells )
    {
      Cell& cell = **it_cell;
      cell.move( *gs );
    }

    /*
     * 細胞分裂をする。
     *
     * 細胞が閾値以上のエネルギーを所持していれば、
     * 同じ位置に新しい細胞を作成する。
     * エネルギーは、半分分け与える。
     */
    VECTOR(Cell *) new_cells;
    EACH( it_cell, cells )
    {
      Cell& origincell = **it_cell;
      double origin_energy = origincell.energy();
      if( origin_energy > CELL_DIVISION_THRESHOLD_ENERGY )
      {
        Cell *newcell = new Cell();

        // 同じ位置に分裂する。
        int newx = origincell.x(); int newy = origincell.y();
        newcell->setLocation( newx, newy );

        // 半分にエネルギーを分ける。
        newcell->setEnergy( origin_energy / 2 );
        origincell.setEnergy( origin_energy / 2 );

        new_cells.push_back( newcell );
      }
    }
    cells.insert(cells.end(), new_cells.begin(), new_cells.end());
    

    /*
     * 細胞が代謝する
     */
    EACH( it_cell, cells )
    {
      Cell& cell = **it_cell;
      cell.metabolize( *gs, *os );
    }

    /*
     * 死細胞を除去する。
     */
    FOREACH( it_cell, cells )
    {
      Cell& cell = **it_cell;
      if( cell.energy() <= CELL_DEATH_THRESHOLD_ENERGY )
      {
        SAFE_DELETE( *it_cell );
        cells.erase( it_cell );
      } else { it_cell++; }
    }

    // グルコーススケープが再生する。
    gs->generate();
    os->generate();

    /* ファイルに出力する */
    // 細胞の分布を出力する
    //output_cell_map( cells );
    output_map_with_value( "cell", cells );
    // 細胞の平均エネルギーを出力する。
    output_cell_energy_average( cells );

    if( stepKeeper.isInterval(1)) {
      // グルコースマップを出力する。
      output_glucose_map( *gs );
      output_oxygen_map( *os );
    }

    /* 細胞数を出力する */
    output_value_with_step("cell-size.txt", cells.size());
  }
  // ------------------------------------------------------

  return 0;
}

/*
 * Function
 */

void output_cell_energy_average( VECTOR(Cell *)& cells ) {
  int sum = 0;
  EACH(it_cell, cells) {
    Cell& cell = **it_cell;
    sum += cell.energy();
  }
  double average = (double)sum/cells.size();
  output_value_with_step("cell-energy-average.txt", average);
}


void output_glucose_map( GlucoseScape& gs ) {
  char file_name[256];
  sprintf(file_name, "%d-glucose.txt", StepKeeper::Instance().step());
  std::ofstream glucose_map_ofs(file_name);

  FOR(i, HEIGHT) {
    FOR(j, WIDTH) {
      glucose_map_ofs << i << SEPARATOR;
      glucose_map_ofs << j << SEPARATOR;
      glucose_map_ofs << gs.glucose(i, j);
      glucose_map_ofs << std::endl;
    }
    glucose_map_ofs << std::endl;
  }
}

void output_oxygen_map( OxygenScape& os ) {
  char file_name[256];
  sprintf(file_name, "%d-oxygen.txt", StepKeeper::Instance().step());
  std::ofstream oxygen_map_ofs(file_name);

  FOR(i, HEIGHT) {
    FOR(j, WIDTH) {
      oxygen_map_ofs << i << SEPARATOR;
      oxygen_map_ofs << j << SEPARATOR;
      oxygen_map_ofs << os.oxygen(i, j);
      oxygen_map_ofs << std::endl;
    }
    oxygen_map_ofs << std::endl;
  }
}

/*
 * Landscape
 */

bool __Landscape::isExistingPoint(int x, int y) {
  if( x < 0 ) return false;
  if( y < 0 ) return false;
  if( x > WIDTH-1 ) return false;
  if( y > HEIGHT-1 ) return false;
  return true;
}

// シュガースケープのクラスを作成する。
// シュガーを生産できる。
class __SugarScape : public __Landscape {
  public:
    virtual void generate() { }
    virtual void material() { } /// TODO
  private:
};

/*
 * StepKeeper
 */
StepKeeper& StepKeeper::Instance() {
  static StepKeeper singleton;
  return singleton;
}

bool StepKeeper::loop() {
  proceed();
  if( step() <= maxStep() ) return true;
  else return false;
}    
bool StepKeeper::isInterval( int interval ) {
  if(step()%interval == 0) return true;
  else return false;
}

/*
 * Cell
 */
Cell::Cell() {
  energy_ = INITIAL_CELL_ENERGY;
  state_ = &( NormalCellState::Instance() );
}

void Cell::metabolize( GlucoseScape& gs, OxygenScape& os ) {
  state_->metabolize( *this, gs, os );
}