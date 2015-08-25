/**
 *
 * 正常細胞とがん細胞との、糖代謝の違いを表現するモデル。
 *
 * 細胞、グルコーススケープ、酸素スケープ
 *
 * クラスにおけるコンストラクタは1つだけにする。ややこしいので。
 * 同様にオーバーロード少なめに
 *
 * memo:
 *   - T細胞
 *   - 細胞は、マテリアルが多い方向に進むか？
 *   - WIDTH, HEIGHTを内部から消す。
 *   
 * TODO:
 *   - (x,y)を(i,j)表記に統一する。
 *
 * @author Naoki Ueda
 *
 */
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <cassert>

// ===========================================================================
/*
 * ターミナル画面制御用
 */
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

/*
 * 汎用マクロ
 */
// #define ASSERT(x)               do { assert(x); }while(0);
#define ASSERT(x)               if(!x) { do { std::cerr<<RED<<"[ ASSERT! ] " \
  <<CLR_ST<<#x<<" <== L"<<__LINE__<<" " \
  <<""<<__FILE__<<std::endl; }while(0);}
#define FOR(i, n)               for(int (i)=0; (i)<(n); (i)++) // i: 0 ~ (n-1)
#define REP(i, min, max)        for(int (i)=(min); (i)<=(max); (i)++)

#define ECHO(x)                 do { std::cout<< CLEAR_RIGHT << "----> " \
  <<GREEN<<x<<STANDARD<<CLR_ST<<"" \
  <<std::endl; }while(0);
#define DEBUG(x)                do { std::cerr<<BOLD<<"[ DEBUG ] " \
  <<CLR_ST<<STANDARD<<#x<<" <-- "<<BOLD<<(x) \
  <<STANDARD<<" (L"<<__LINE__<<")" \
  <<""<<__FILE__<<std::endl; }while(0);
#define PIN                     do { static int point = 0; std::cerr \
  <<BOLD<<RED<<"[ PIN ] "<<CLR_ST \
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
// ===========================================================================

/*
 * 定数パラメータを定義する。
 */
typedef double MATERIAL;
typedef double ENERGY;
typedef std::string GENE;
typedef double PROBABILITY;

// ランドスケープの幅と高さを設定する。
const int WIDTH  = 30; //: 幅
const int HEIGHT = 30; //: 高さ

/* グルコース, 酸素の再生量 /1step */
const MATERIAL GLUCOSE_GENERATE = 1; //: グルコース再生量
const MATERIAL OXYGEN_GENERATE = 1; //: 酸素再生量
const MATERIAL MAX_GLUCOSE = 20; //: 最大グルコース量
const MATERIAL MAX_OXYGEN = 20; //: 最大酸素量

// 最大計算期間を設定する。
const int MAX_STEP = 3000; //: 最大ステップ数

// 細胞数を設定する。
const int CELL_SIZE = 100; //: 初期総細胞数
const int TCELL_SIZE = 1000; //: T初期総細胞数
const int TCELL_LIFESPAN = 10; //: T細胞の寿命

// 使用量
const MATERIAL NORMALCELL_METABOLIZE_GLUCOSE = 1; //: 正常細胞代謝時グルコース使用量
const MATERIAL NORMALCELL_METABOLIZE_OXYGEN = 1; //: 正常細胞代謝時酸素使用量
const MATERIAL CANCER_CELL_METABOLIZE_GLUCOSE = 2; //: がん細胞代謝時グルコース使用量

// 代謝量
const ENERGY NORMAL_CELL_GAIN_ENERGY = 5; //: 正常細胞代謝量
const ENERGY CANCER_CELL_GAIN_ENERGY = 2; //: がん細胞代謝量

/*
 * 細胞に関するパラメータ
 */
const ENERGY INITIAL_CELL_ENERGY = 20; //: 初期細胞エネルギー

const ENERGY CELL_DEATH_THRESHOLD_ENERGY = 0; //: 細胞アポトーシスエネルギー閾値
const ENERGY CELL_DIVISION_THRESHOLD_ENERGY = 10; //: 細胞分裂エネルギー閾値

const int MAX_CELL_DIVISION_COUNT = 10; //: 通常細胞の最大分裂回数

const PROBABILITY CELL_MUTATION_RATE = 1; //: 細胞突然変異確率

const int CELL_GENE_LENGTH = 8; //: 遺伝子の長さ

const PROBABILITY NORMALCELL_METABOLIZE_PROB = 20; //: 正常代謝する確率
const PROBABILITY CANCERCELL_METABOLIZE_PROB = 80; //: がん代謝する確率
const PROBABILITY NORMALCELL_DIVISION_PROB = 30; //: 正常細胞分裂確率
const PROBABILITY CANCERCELL_DIVISION_PROB = 30; //: がん細胞分裂確率
// 移動する確率
// const PROBABILITY MOTILITY_PROB
const double MOTILITY_WEIGHT = 1; //: 移動にかかるコストの重み

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

    int width() const;  // 幅を返す
    int height() const; // 高さを返す

    // ランドスケープ上に存在する点かどうかを評価する。
    bool isExistingPoint( int x, int y );
  private:
    int width_, height_;
};

int __Landscape::width() const { return width_; }
int __Landscape::height() const { return height_; }

/**
 * @brief シュガースケープのインターフェイス
 *
 * シュガーを生産できる。
 */
class __SugarScape : public __Landscape {
  public:
    virtual void generate() = 0;  // シュガーを再生する
    virtual MATERIAL material(int x, int y) const = 0;  // シュガーの量を返す
  private:
};

/**
 * @brief グルコースのクラス
 */
class GlucoseScape : public __SugarScape {
  public:
    GlucoseScape();

    virtual void generate();              // 再生する
    MATERIAL glucose(int x, int y) const; // グルコースの量を返す
    virtual MATERIAL material(int x, int y) const;  // グルコースの量を返す
    void setGlucose(int x, int y, MATERIAL value);  // グルコースの量を設定する
  private:
    MATERIAL glucose_map_[HEIGHT][WIDTH];  // グルコースマップ配列
};
/**
 * @brief 酸素のクラスを作成する。
 */
class OxygenScape : public __SugarScape {
  public:
    OxygenScape();

    MATERIAL oxygen(int x, int y) const;  // 酸素の量を返す
    virtual MATERIAL material(int x, int y) const;  // 酸素の量を返す
    void setOxygen(int x, int y, MATERIAL value);   // 酸素の量を設定する
    virtual void generate();                        // 再生する
  private:
    MATERIAL oxygen_map_[HEIGHT][WIDTH];  // 酸素マップ配列
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

    /**
     * 移動する。
     *
     * ランドスケープ上を移動する。
     * 壁あり。
     *
     * @param landscape スケープ
     * @return 移動した距離を、マンハッタン距離で返す。
     */
    virtual double move( __Landscape& landscape );
    int movementDistance() const { return movement_distance_; }

  private:
    int movement_distance_;  // 移動距離変数
};

class __CellState;
class NormalCellState;
class CancerCellState;


/**
 * @brief 生命クラス
 *
 * 遺伝子を持つ。
 */
class __Life {
public:
  /** 遺伝子配列を返す */
  GENE gene();

  void setGene( GENE gene ) { gene_ = gene; }

  /** 遺伝子の値を返す */
  int geneValue();

  /** 遺伝子配列を初期化する */
  void initiateGene( int length );

  /** 遺伝子配列をランダムに設定する */
  void randomSetGene( int length );

  /** フリップする */
  void flip( int pos );

  /** 突然変異する */
  bool mutateGene( double prob );

  /** 遺伝子が同一の配列かどうかを判定する */
  bool match( __Life& life );

private:
  GENE gene_;  // 遺伝子文字列
};

/**
 * @brief 細胞クラス
 *
 * 代謝する。
 * エネルギーを持つ。
 */
class Cell : public __Mobile, public __Life {
 public:
  Cell();
  virtual ~Cell() { }

  ENERGY energy() const { return energy_; }
  void setEnergy( ENERGY energy ) { energy_ = energy; }
  void consumeEnergy( ENERGY consume ) { setEnergy( energy() - consume ); }
  void gainEnergy( ENERGY gather ) { setEnergy( energy() + gather ); }

  /** 代謝する */
  void metabolize( GlucoseScape& gs, OxygenScape& os );

  // __CellState& cellState();
  // void changeState();

  /** がん細胞かどうかを返す */
  // 遺伝子の評価値が１以上ならば、がん細胞
  bool isCancerCell();
  bool isNormalCell();

  void incrementDivisionCount() { cell_division_count_++; }
  int divisionCount() { return cell_division_count_; }
  bool willDvision();

  /** スケープ上を移動する */
  virtual double move( __Landscape& landscape );

  bool willDie() {
    if( energy() <= CELL_DEATH_THRESHOLD_ENERGY ) return true;
    if( divisionCount() >= MAX_CELL_DIVISION_COUNT ) return true;
    return false;
  }

  /**
   * 指定した確率で突然変異する。
   *
   * @param prob 突然変異確率
   */
  void mutate( double prob );

  int immunogenicity();

 private:
  ENERGY energy_;
  // __CellState *state_;
  int cell_division_count_;

  // 免疫原性率 0 ~ 100 %
  //int immunogenicity_;
};

int Cell::immunogenicity() {
  int ret = 0;
  // if(gene()[0]=='1') return 20;
  // return 100;
  ret = 100*geneValue()/CELL_GENE_LENGTH;
  return ret;
}

class Tcell : public __Mobile, public __Life {
public:
  Tcell();
  virtual ~Tcell() { }

  int age();
  void setAge( int age ) { age_ = age; }
  void aging();
  void initAge();

  Tcell& clone() {
    Tcell *newtcell = new Tcell();  // 新しいT細胞を作成する。
    newtcell->setGene( gene() );    // 遺伝子を設定して、
    newtcell->setX(x());            // 座標を
    newtcell->setY(y());            // 同じ位置にして、
    return *newtcell;               // 返す。
  }

private:
  int age_;
};

Tcell::Tcell() {
  age_ = 0;
}
int Tcell::age() { return age_; }
void Tcell::aging() {
  age_ += 1;
}
void Tcell::initAge() { age_ = 0; }

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
  virtual bool isNormalCell() = 0;
  virtual bool isCancerCell() = 0;
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
    MATERIAL g = gs.glucose(cell.x(), cell.y());
    MATERIAL o = os.oxygen(cell.x(), cell.y());
    MATERIAL use_glucose = NORMALCELL_METABOLIZE_GLUCOSE;
    MATERIAL use_oxygen = NORMALCELL_METABOLIZE_OXYGEN;
    if( g >= use_glucose && o >= use_oxygen ) {
      cell.gainEnergy( NORMAL_CELL_GAIN_ENERGY );
      gs.setGlucose( cell.x(), cell.y(), g - use_glucose );
      os.setOxygen( cell.x(), cell.y(), o - use_oxygen );
    }
  }

  /** 正常細胞なら真を返す */
  virtual bool isNormalCell() { return true; }

  /** がん細胞なら真を返す */
  virtual bool isCancerCell() { return false; }
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
  static CancerCellState& Instance() {
    static CancerCellState singleton;
    return singleton;
  }

  /**
   * グルコースのみを利用してエネルギーを産生する。
   *
   * @param cell 細胞
   * @param gs グルコーススケープ
   * @param os 酸素スケープ
   */
  virtual void metabolize( Cell& cell,  GlucoseScape& gs, OxygenScape& os ) {
    MATERIAL g = gs.glucose(cell.x(), cell.y());
    MATERIAL use_glucose = CANCER_CELL_METABOLIZE_GLUCOSE;
    if( g >= use_glucose ) {
      cell.gainEnergy( CANCER_CELL_GAIN_ENERGY );
      gs.setGlucose( cell.x(), cell.y(), g-use_glucose );
    }
  }

  virtual bool isNormalCell() { return false; }
  virtual bool isCancerCell() { return true; }
private:
};

/**
 * @brief 細胞のマップクラス
 */
class TcellMap {
public:
  TcellMap() { }
  ~TcellMap();

  /** マップをリセットする */
  void resetMap() {
    FOR( i, HEIGHT ) {
      FOR( j, WIDTH ) {
        tcell_map_[i][j].clear();
      }
    }
  }

  /** T細胞の位置を登録する */
  void resistTcells( VECTOR(Tcell *) tcells ) {
    resetMap();
    EACH( it_tcell, tcells ) {
      Tcell &tcell = **it_tcell;
      int i = tcell.y();
      int j = tcell.x();
      tcell_map_[i][j].push_back( &tcell );
    }
  }

  /** 指定した位置のT細胞配列を返す */
  VECTOR(Tcell *)& tcellsAt( int i, int j ) {
    return tcell_map_[i][j];
  }

private:
  VECTOR(Tcell *) tcell_map_[HEIGHT][WIDTH];
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
void output_value_with_step( const char *fname, T value );

/*
 * ステップ数と一緒に、その時のマップを出力する関数
 */
template < typename T >
void output_map_with_value( const char *fname, VECTOR(T *)& agents );
void output_normalcell_map_with_value( const char *fname,  VECTOR(Cell *)& cells );
void output_cancercell_map_with_value( const char *fname,  VECTOR(Cell *)& cells );

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


// ============================================================================
//
// エントリーポイント
//
// ============================================================================
int main() {
  ECHO("Cancer Immunoediting Model");

  // 期間クラスのインスタンスを生成する
  StepKeeper &stepKeeper = StepKeeper::Instance();
  stepKeeper.setMaxStep( MAX_STEP );

  // グルコース、酸素マップのインスタンスを作成する。
  GlucoseScape *gs = new GlucoseScape();
  OxygenScape *os = new OxygenScape();

  TcellMap *tcellmap = new TcellMap();

  // 細胞を初期化していく。
  // TODO: 普通の細胞は細胞土地のほうがいいかも
  VECTOR(Cell *) cells;
  FOR(i, CELL_SIZE) {
    // 新しい細胞を作成。
    // 位置を設定する
    // 遺伝子を初期化する
    // 配列に加える
    Cell *newcell = new Cell();
    newcell->randomSetLocation();
    newcell->setEnergy( Random::Instance().uniformInt(0, INITIAL_CELL_ENERGY) );
    cells.push_back( newcell );
  }

  // T細胞を初期化していく。
  VECTOR(Tcell *) tcells;
  FOR( i, TCELL_SIZE ) {
    Tcell *tc = new Tcell();
    tc->randomSetLocation();
    tc->randomSetGene( CELL_GENE_LENGTH );
    tc->setAge( Random::Instance().uniformInt(0, TCELL_LIFESPAN ));
    tcells.push_back( tc );
  }

  // 計算を実行する ---------------------------------------
  while( stepKeeper.loop() )
  {
    if( stepKeeper.isInterval(100) ) {
      DEBUG(stepKeeper.step());
    }
    /*
     * 細胞、T細胞を移動させる。
     */
    EACH( it_cell, cells )
    {
      Cell& cell = **it_cell;
      cell.move( *gs );
    }
    EACH( it_tcell, tcells )
    {
      Tcell& tcell = **it_tcell;
      tcell.move( *gs );
    }

    // 細胞の位置などを登録する
    tcellmap->resistTcells( tcells );

    /*
     * 細胞分裂をする。
     *
     * 細胞が閾値以上のエネルギーを所持していれば、
     * 同じ位置に新しい細胞を作成する。
     * エネルギーは、半分分け与える。
     */
    int normaldivisioncount = 0;
    int cancerdivisioncount = 0;
    int mutationcount = 0;
    VECTOR(Cell *) new_cells;
    EACH( it_cell, cells ) {
      Cell& origincell = **it_cell;

      // 分裂不可能ならスキップする。
      if( origincell.willDvision() == false ) {
        continue;
      }

      ENERGY origin_energy = origincell.energy();
      if( origin_energy > CELL_DIVISION_THRESHOLD_ENERGY ) {
        Cell *newcell = new Cell();

        // 同じ位置に分裂する。
        int newx = origincell.x(); int newy = origincell.y();
        newcell->setLocation( newx, newy );

        // 遺伝子配列を同じにする。
        // がん細胞からはがん細胞が分裂する。
        // 正常細胞からは、がん細胞が分裂する可能性がある
        newcell->setGene( origincell.gene() );
        ASSERT( origincell.match(*newcell) );
        if( origincell.isNormalCell() ) {
          normaldivisioncount++;
        } else {
          cancerdivisioncount++;
        }

        // 突然変異する
        if( newcell->mutateGene( CELL_MUTATION_RATE ) ) { mutationcount++; } // 突然変異をしたらカウントする

        // 半分にエネルギーを分ける。
        newcell->setEnergy( origin_energy / 2 );
        origincell.setEnergy( origin_energy / 2 );

        new_cells.push_back( newcell );
        origincell.incrementDivisionCount();  // 分裂回数を増やす。
      }
    }
    cells.insert(cells.end(), new_cells.begin(), new_cells.end()); // 配列に加える。


    /*
     * 細胞が代謝する
     */
    EACH( it_cell, cells ) {
      Cell& cell = **it_cell;
      cell.metabolize( *gs, *os );
    }

    /*
     * 死細胞を除去する。
     */
    FOREACH( it_cell, cells ) {
      Cell& cell = **it_cell;
      if( cell.willDie() ) {
        SAFE_DELETE( *it_cell );
        cells.erase( it_cell );
      } else { it_cell++; }
    }

    /*
     * 免疫で除去する
     *
     * T細胞と同じ位置に、
     * がん細胞かつ認識するがん細胞がある場合、
     * そのがん細胞を細胞配列から除去する。
     */
    int deletedcellssize = 0;
    VECTOR(Tcell *) newtcells;
    FOREACH( it_cell, cells )
    {
      Cell& cell = **it_cell;
      int i = cell.y(); int j = cell.x();

      // がん細胞であれば、
      // T細胞によって排除されるか判定される
      if( cell.isCancerCell() ) {
        VECTOR(Tcell *) tcells = tcellmap->tcellsAt( i, j );
        if( tcells.size() > 0 )
        {
          bool matching = false;
          EACH( it_tcell, tcells )
          {
            Tcell& tcell = **it_tcell;

            // 免疫原性の確率で、
            // 遺伝子配列が一致していれば、
            // 除去する。
            if( Random::Instance().probability( cell.immunogenicity() ) and cell.match( tcell ) )
            {
              SAFE_DELETE( *it_cell );
              cells.erase( it_cell );
              deletedcellssize++;
              matching = true;

              newtcells.push_back( &tcell.clone() );
              break;
            }
          }
          if( matching == false ) it_cell++;
        } else { it_cell++; }
      } else { it_cell++; }
    }

    // グルコーススケープが再生する。
    gs->generate();
    os->generate();

    /*
     * T細胞が老化する
     */
    int tcellsize = 0;  // T細胞の総数をカウント
    int inittcellsize = 0;  // T細胞が初期化された回数をカウント
    VECTOR(Tcell *) newborntcells;
    FOREACH( it_tcell, tcells )
    {
      Tcell &tcell = **it_tcell;
      tcell.aging();

      if( tcell.age() >= TCELL_LIFESPAN ) {
        SAFE_DELETE( *it_tcell );
        tcells.erase( it_tcell );
        inittcellsize++;

        // 新しいT細胞を加える。
        // Tcell *tc = new Tcell();
        // tc->randomSetLocation();  // 位置はランダム
        // tc->randomSetGene( CELL_GENE_LENGTH );  // 遺伝子配列もランダム
        // newborntcells.push_back( tc );
      } else {
        it_tcell++; 
        tcellsize++;
      }        
    }

    /*
     * T細胞を補完する。
     */
    int short_tcell_size = TCELL_SIZE - tcellsize;
    FOR( i, std::max( 0, short_tcell_size ) ) {
      Tcell *tc = new Tcell();
      tc->randomSetLocation();  // 位置はランダム
      tc->randomSetGene( CELL_GENE_LENGTH );  // 遺伝子配列もランダム
      tcells.push_back( tc );
      tcellsize++;
    }
    tcells.insert(tcells.end(), newtcells.begin(), newtcells.end()); // 配列に加える。
    tcells.insert(tcells.end(), newborntcells.begin(), newborntcells.end()); // 配列に加える。

    // EACH( it_tcell, tcells )
    // {
    //   Tcell &tcell = **it_tcell;
    //   tcell.aging();

    //   // T細胞が寿命なら、
    //   // 初期化する。
    //   // 遺伝子を再初期化して、
    //   // 細胞年齢を初期化する。
    //   if( tcell.age() >= TCELL_LIFESPAN ) {
    //     tcell.randomSetGene( CELL_GENE_LENGTH );
    //     tcell.initAge();

    //     inittcellsize++;
    //   }
    //   tcellsize++;
    // }

    // -----------------------------------------------------------------------
    /* ファイルに出力する */
    // 細胞の分布を出力する
    //output_cell_map( cells );
    output_map_with_value( "cell", cells );
    output_normalcell_map_with_value( "normalcell", cells );
    output_cancercell_map_with_value( "cancercell", cells );
    output_map_with_value( "tcell", tcells );

    // 細胞の平均エネルギーを出力する。
    output_cell_energy_average( cells );

    // 突然変異がん細胞の数を出力する
    int normalsize = 0;
    int cancersize = 0;
    int mutantcancercellsize = 0;
    int standardcancercellsize = 0;
    EACH( it_cell, cells ) {
      Cell& cell = **it_cell;
      if( cell.isCancerCell() ) {
        if( cell.gene()[0] == '1' ) { mutantcancercellsize++; }
        else { standardcancercellsize++; }
      }
      if( cell.isNormalCell() ) {
        normalsize++;
      } else { cancersize++; }
    }
    output_value_with_step("mutantcancer-size.txt", mutantcancercellsize);
    output_value_with_step("standardcancer-size.txt", standardcancercellsize);

    if( stepKeeper.isInterval(1)) {
      // グルコースマップを出力する。
      output_glucose_map( *gs );
      output_oxygen_map( *os );
    }

    /* 細胞数を出力する */
    // int normalsize = 0;
    // int cancersize = 0;
    // EACH( it_cell, cells ) {
    //   Cell& cell = **it_cell;
    //   if( cell.isNormalCell() ) {
    //     normalsize++;
    //   } else cancersize++;
    // }
    output_value_with_step("normalcell-size.txt", normalsize);
    output_value_with_step("cancercell-size.txt", cancersize);
    output_value_with_step("deleted-cell-size.txt", deletedcellssize);
    output_value_with_step("tcell-size.txt", tcells.size() );
    output_value_with_step("init-tcell-size.txt", inittcellsize);
    output_value_with_step("mutation-count.txt", mutationcount);
    output_value_with_step("normal-division-count.txt", normaldivisioncount);
    output_value_with_step("cancer-division-count.txt", cancerdivisioncount);
  }
  // ------------------------------------------------------

  return 0;
}

// ============================================================================
//
// Definition
//
// ============================================================================

/*
 * Function
 */
template < typename T >
void output_value_with_step( const char *fname, T value ) {
  int step = StepKeeper::Instance().step();
  std::ofstream ofs(fname, std::ios_base::out | std::ios_base::app);
  ofs << step << SEPARATOR;
  ofs << value << std::endl;
};

template < typename T >
void output_map_with_value( const char *fname,  VECTOR(T *)& agents ) {
  // ファイル名
  char file_name[256];
  sprintf(file_name, "%d-%s.txt", StepKeeper::Instance().step(), fname);
  std::ofstream agent_map_ofs(file_name);

  // マップの全ての位置を0で初期化する。
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

void output_normalcell_map_with_value( const char *fname,  VECTOR(Cell *)& cells ) {
  // ファイル名
  char file_name[256];
  sprintf(file_name, "%d-%s.txt", StepKeeper::Instance().step(), fname);
  std::ofstream agent_map_ofs(file_name);

  // マップの全ての位置を0で初期化する。
  int agent_map[HEIGHT][WIDTH] = {};
  EACH(it_cell, cells) {
    Cell& cell = **it_cell;
    if( cell.isNormalCell() == false ) continue;
    agent_map[cell.y()][cell.x()]++;
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
void output_cancercell_map_with_value( const char *fname,  VECTOR(Cell *)& cells ) {
  // ファイル名
  char file_name[256];
  sprintf(file_name, "%d-%s.txt", StepKeeper::Instance().step(), fname);
  std::ofstream agent_map_ofs(file_name);

  // マップの全ての位置を0で初期化する。
  int agent_map[HEIGHT][WIDTH] = {};
  EACH(it_cell, cells) {
    Cell& cell = **it_cell;
    if( cell.isCancerCell() == false ) continue;
    agent_map[cell.y()][cell.x()]++;
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

void output_cell_energy_average( VECTOR(Cell *)& cells ) {
  int sum = 0;
  int normalsum = 0;
  int cancersum = 0;
  int normalsize = 0;
  int cancersize = 0;
  EACH(it_cell, cells) {
    Cell& cell = **it_cell;
    sum += cell.energy();
    if(cell.isNormalCell()) {
      normalsum += cell.energy();
      normalsize++;
    }
    else { 
      cancersum += cell.energy(); 
      cancersize++;
    }
  }
  double average = 0;
  double normalave = 0;
  double cancerave = 0;
  if( cells.size() > 0 ) average = (double)sum/cells.size();
  if( normalsize > 0 ) normalave = (double)normalsum/normalsize;
  if( cancersize > 0 ) cancerave = (double)cancersum/cancersize;

  output_value_with_step("cell-energy-average.txt", average);
  output_value_with_step("normal-energy-average.txt", normalave);
  output_value_with_step("cancer-energy-average.txt", cancerave);
}


void output_glucose_map( GlucoseScape& gs ) {
  char file_name[256];
  sprintf(file_name, "%d-glucose.txt", StepKeeper::Instance().step());
  std::ofstream glucose_map_ofs(file_name);

  FOR(i, HEIGHT) {
    FOR(j, WIDTH) {
      glucose_map_ofs << i << SEPARATOR;
      glucose_map_ofs << j << SEPARATOR;
      glucose_map_ofs << gs.glucose(j, i);
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
      oxygen_map_ofs << os.oxygen(j, i);
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
 * GlucoseScape
 */
GlucoseScape::GlucoseScape() {
  // 全てのマップに初期グルコース量を配置する。
  FOR(i, HEIGHT) {
    FOR(j, WIDTH) {
      glucose_map_[i][j] = 5;
    }
  }
}
void GlucoseScape::generate() {
  FOR(i, HEIGHT) {
    FOR(j, WIDTH) {
      if(glucose(j, i) <= MAX_GLUCOSE - GLUCOSE_GENERATE) {
        glucose_map_[i][j] += GLUCOSE_GENERATE;
      }
    }
  }
}

MATERIAL GlucoseScape::glucose(int x, int y) const { return glucose_map_[y][x]; }
MATERIAL GlucoseScape::material(int x, int y) const { return glucose(x, y); }
void GlucoseScape::setGlucose(int x, int y, MATERIAL value) { glucose_map_[y][x] = value; }

/*
 * OxygenScape
 */
MATERIAL OxygenScape::oxygen(int x, int y) const { return oxygen_map_[y][x]; }
MATERIAL OxygenScape::material(int x, int y) const { return oxygen(x, y); }
void OxygenScape::setOxygen(int x, int y, MATERIAL value) { oxygen_map_[y][x] = value; }
void OxygenScape::generate() {
  FOR(i, HEIGHT) {
    FOR(j, WIDTH) {
      if(oxygen(j, i) <= MAX_OXYGEN - OXYGEN_GENERATE) {
        oxygen_map_[i][j] += OXYGEN_GENERATE;
      }
    }
  }
}

OxygenScape::OxygenScape() {
  // 全てのマップに初期酸素量を配置する。
  FOR(i, HEIGHT) {
    FOR(j, WIDTH) {
      oxygen_map_[i][j] = 5;
    }
  }
}

/*
 * Cell
 */
Cell::Cell() {
  // energy_ = Random::Instance().uniformInt(0, INITIAL_CELL_ENERGY);
  setEnergy( INITIAL_CELL_ENERGY );
  // state_ = &( NormalCellState::Instance() );
  cell_division_count_ = 0;

  //immunogenicity_ = 0;
  //if( cellState().isNormalCell() ) immunogenicity_ = 0;
  //if( cellState().isCancerCell() ) immunogenicity_ = 50;

  initiateGene( CELL_GENE_LENGTH );
}

// void Cell::changeState() {
//   state_ = &( CancerCellState::Instance() );
// }

void Cell::metabolize( GlucoseScape& gs, OxygenScape& os ) {
  // state_->metabolize( *this, gs, os );
  if( isNormalCell() and Random::Instance().probability(NORMALCELL_METABOLIZE_PROB) ) {
    NormalCellState::Instance().metabolize(*this, gs, os);
    return;
  }
  if( isCancerCell() and Random::Instance().probability(CANCERCELL_METABOLIZE_PROB) ) {
    CancerCellState::Instance().metabolize(*this, gs, os);
    return;
  }
}

// void Cell::mutate( double prob ) {
//   if(Random::Instance().probability(prob)) {
//     /*
//      * 正常細胞なら、がん細胞に変異する。
//      */
//     if( state_->isNormalCell() ) {
//       changeState();
//     }
//   }
// }

bool Cell::isCancerCell() {
  if( geneValue() > 0 ) { return true; }
  else { return false; }
}
bool Cell::isNormalCell() {
  if( geneValue() <= 0 ) { return true; }
  else { return false; }
}
double Cell::move( __Landscape& landscape ) {
  double distance = __Mobile::move(landscape);
  consumeEnergy( distance * MOTILITY_WEIGHT );
  return distance;
}


/**
 * 分裂可能かを返す
 *
 * @return 真偽値
 */
bool Cell::willDvision() {
  // がん細胞なら無条件で分裂可能にする。
  if( isCancerCell() and Random::Instance().probability(CANCERCELL_DIVISION_PROB) ) return true;
  if( isNormalCell() and Random::Instance().probability(NORMALCELL_DIVISION_PROB) ) {
    if( divisionCount() < MAX_CELL_DIVISION_COUNT ) {
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

// __CellState& Cell::cellState() {
//   if( isNormalCell() ) {
//     return NormalCellState::Instance();
//   } else {
//     return CancerCellState::Instance();
//   }
// }

/*
 * __Mobile
 */
double __Mobile::move( __Landscape& landscape ) {
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

/*
 * __Life
 */
GENE __Life::gene() { return gene_; }
int __Life::geneValue() {
  int ret = 0;
  FOR( i, CELL_GENE_LENGTH ) {
    if( gene_[i] == '1' ) {
      ret++;
    }
  }
  return ret;
}
void __Life::randomSetGene( int length ) {
  gene_ = "";
  FOR( i, length ) {
    gene_ += Random::Instance().probability(50) ? '0' : '1';
  }
}

void __Life::initiateGene( int length ) {
  gene_ = "";
  FOR( i, length ) {
    gene_ += '0';
  }
}
void __Life::flip( int pos ) {
  pos = pos%CELL_GENE_LENGTH;
  if( gene_[pos] == '0' ) {
    gene_[pos] = '1';
  } else {
    gene_[pos] = '0';
  }
}

bool __Life::mutateGene( double prob ) {
  // 突然変異をしたら、真を返す
  // 0の時だけ1にする
  bool changed = false;
  if( Random::Instance().probability(prob) ) {
    int pos = Random::Instance().uniformInt( 0, CELL_GENE_LENGTH-1 );
    // flip(pos);
    pos = pos%CELL_GENE_LENGTH;
    if( gene_[pos] == '0' ) {
      gene_[pos] = '1';
      changed = true;
    }
  }
  return changed;
}
bool __Life::match( __Life& life ) {
  if( gene() == life.gene() ) { return true; }
  else { return false; }
}
