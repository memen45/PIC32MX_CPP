//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// DO NOT INCLUDE THIS FILE (except in Usb.hpp) - used inline in Usb.hpp
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

struct UsbBdt {
//______________________________________________________________________________
//
// all BDT table functions
// bdt table allocated here, Usb:: functions set address of bdt table
// init functions also return address of table (init_all) or table entry (init)
// something like-
//  uint32_t* ba = UsbBdt::init_all();
//  Usb::bdt_addr(ba);
//______________________________________________________________________________

    enum CTRL {
        UOWN = 1<<7, DATA01 = 1<<6, KEEP = 1<<5,
        NINC = 1<<4, DTS = 1<<3, BSTALL = 1<<1
    };

    typedef union {
        struct { uint32_t dat; uint32_t addr; };
        struct { unsigned :16; uint16_t count; };
        struct {
            unsigned :2;
            unsigned bstall:1;
            unsigned dts:1;
            unsigned ninc:1;
            unsigned keep:1;
            unsigned data01:1;
            unsigned uown:1;
        };
        struct { unsigned :2; unsigned pid:4; };
        uint8_t control;
        uint64_t all;
    } bdt_t;

    static uint32_t    init        (uint8_t);
    static uint32_t    init_all    ();

    static void        esetup      (uint8_t, uint8_t*, uint16_t, uint8_t);

    static void        addr        (uint8_t, uint8_t*);
    static uint8_t*    addr        (uint8_t);
    static void        control     (uint8_t, CTRL, bool);
    static void        control     (uint8_t, uint8_t);
    static bool        control     (uint8_t, CTRL);
    //control - specific functions (without needing to specify CTRL)
    static bool        uown        (uint8_t);
    static bool        data01      (uint8_t);
    static bool        stall       (uint8_t);
    static void        uown        (uint8_t, bool);
    static void        data01      (uint8_t, bool);
    static void        stall       (uint8_t, bool);

    static uint8_t     pid         (uint8_t);
    static uint16_t    count       (uint8_t);
    static void        count       (uint8_t, uint16_t);
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// UsbBdt static functions, vars
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
static volatile UsbBdt::bdt_t bdt[(my_last_endp+1)*4]
    __attribute__ ((aligned (512)));

//init single bdt table entry (epn<<2|dir<<1|pp)
uint32_t UsbBdt::init(uint8_t n){
    if(n > (my_last_endp*4)) return 0; //n too high, do nothing
    bdt[n].all = 0;
    return (uint32_t)&bdt[n];
}
//init all bdt table entries (4 per endpoint)
uint32_t UsbBdt::init_all(){
    for(auto& i : bdt) i.all = 0;
    return (uint32_t)bdt;
}

void UsbBdt::esetup(uint8_t n, uint8_t* a, uint16_t c, uint8_t f){
    addr(n, a);
    count(n, c);
    control(n, f);
}

void UsbBdt::addr(uint8_t n, uint8_t* v){ bdt[n].addr = Reg::k2phys(v); }
uint8_t* UsbBdt::addr(uint8_t n){ return (uint8_t*)Reg::p2kseg0(bdt[n].addr); }

void UsbBdt::control(uint8_t n, CTRL e, bool tf){
    if( tf ) bdt[n].control |= e;
    else bdt[n].control &= ~e;
}
void UsbBdt::control(uint8_t n, uint8_t v){ bdt[n].control = v; }
bool UsbBdt::control(uint8_t n, CTRL e){ return bdt[n].control & e; }
bool UsbBdt::uown(uint8_t n){ return bdt[n].uown; }
bool UsbBdt::data01(uint8_t n){ return bdt[n].data01; }
bool UsbBdt::stall(uint8_t n){ return bdt[n].bstall; }
void UsbBdt::uown(uint8_t n, bool tf){ bdt[n].uown = tf; }
void UsbBdt::data01(uint8_t n, bool tf){ bdt[n].data01 = tf; }
void UsbBdt::stall(uint8_t n, bool tf){ bdt[n].bstall = tf; }

uint8_t UsbBdt::pid(uint8_t n){ return bdt[n].pid; }

uint16_t UsbBdt::count(uint8_t n){ return bdt[n].count; }
void UsbBdt::count(uint8_t n, uint16_t v){ bdt[n].count = v; }
