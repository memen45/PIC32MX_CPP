#pragma once

//I/O pins

#include <cstdint>

struct Pins {

    //helper enum
    enum : uint8_t {
        PTSHIFT = 4, PTMASK = 7,
        PNSHIFT = 0, PNMASK = 15,
        //RPSHIFT = 6, RPMASK = 31,
        ANSHIFT = 12, ANMASK = 15,
		CNSHIFT = 7, CNMASK = 31, 
		NO_CN = 29
    };

    //PIN11 (physical), A0, RP1, AN0 - use any
    enum RPN : uint16_t {
        // encode as- 0xaaaacccccpppnnnn - | ANn | CNn | PORT | PIN |
        // aaaa = ANn = 0-15
        // ppp = port A=0,B=1,C=2,D=3, E=4, F=5, G=6
        // nnnn = pin = 0-15
        // ccccc = CNn = 1-21 

        // NOTE: pins without AN (analog in) will be set to use AVss (29) as AN input
        // (since AN0 is 0, we need something other than 0 to signify no AN for pin, so
        // we will just give it something that will result in no result if ever used
        // adcval() by mistake)
		// NOTE: pins without CN (change notice) will be set to use AVss (29) as CN nr

        //A0 - A-15
        A0 = 0|NO_CN<<CNSHIFT, A1 = 1|NO_CN<<CNSHIFT,
        A2 = 2|NO_CN<<CNSHIFT, A3 = 3|NO_CN<<CNSHIFT,
        A4 = 4|NO_CN<<CNSHIFT, A5 = 5|NO_CN<<CNSHIFT,
        A6 = 6|NO_CN<<CNSHIFT, A7 = 7|NO_CN<<CNSHIFT,
        /* no A8 = 8,*/ A9 = 9|NO_CN<<CNSHIFT,
        A10 = 10|NO_CN<<CNSHIFT, /* no A11 = 11,
        A12 = 12, A13 = 13,*/
        A14 = 14|NO_CN<<CNSHIFT, A15 = 15|NO_CN<<CNSHIFT,
        //B0-B15
        B0 = 16|2<<CNSHIFT|0<<ANSHIFT, B1 = 17|3<<CNSHIFT|1<<ANSHIFT,
        B2 = 18|4<<CNSHIFT|2<<ANSHIFT, B3 = 19|5<<CNSHIFT|3<<ANSHIFT,
        B4 = 20|6<<CNSHIFT|4<<ANSHIFT, B5 = 21|7<<CNSHIFT|5<<ANSHIFT,
        B6 = 22|6<<ANSHIFT|NO_CN<<CNSHIFT, B7 = 23|7<<ANSHIFT|NO_CN<<CNSHIFT,
        B8 = 24|8<<ANSHIFT|NO_CN<<CNSHIFT, B9 = 25|9<<ANSHIFT|NO_CN<<CNSHIFT,
        B10 = 26|10<<ANSHIFT|NO_CN<<CNSHIFT, B11 = 27|11<<ANSHIFT|NO_CN<<CNSHIFT,
        B12 = 28|12<<ANSHIFT|NO_CN<<CNSHIFT, B13 = NO_CN|13<<ANSHIFT|NO_CN<<CNSHIFT,
        B14 = 30|14<<ANSHIFT|NO_CN<<CNSHIFT, B15 = 31|15<<ANSHIFT|12<<CNSHIFT,
        //C0-C15
        /* no C0 = 32,*/ C1 = 33|NO_CN<<CNSHIFT,
        C2 = 34|NO_CN<<CNSHIFT, C3 = 35|NO_CN<<CNSHIFT,
        C4 = 36|NO_CN<<CNSHIFT, /* no C5 = 37,
        C6 = 38, C7 = 39,
        C8 = 40, C9 = 41,
        C10 = 42, C11 = 43,*/
        C12 = 44|NO_CN<<CNSHIFT, C13 = 45|1<<CNSHIFT,
        C14 = 46|0<<CNSHIFT, C15 = 47|NO_CN<<CNSHIFT,
        //D0-D15
        D0 = 48|NO_CN<<CNSHIFT, D1 = 49|NO_CN<<CNSHIFT,
        D2 = 50|NO_CN<<CNSHIFT, D3 = 51|NO_CN<<CNSHIFT,
        D4 = 52|13<<CNSHIFT, D5 = 53|14<<CNSHIFT,
        D6 = 54|15<<CNSHIFT, D7 = 55|16<<CNSHIFT,
        D8 = 56|NO_CN<<CNSHIFT, D9 = 57|NO_CN<<CNSHIFT,
        D10 = 58|NO_CN<<CNSHIFT, D11 = 59|NO_CN<<CNSHIFT,
        D12 = 60|NO_CN<<CNSHIFT, D13 = 61|19<<CNSHIFT,
        D14 = 62|20<<CNSHIFT, D15 = 63|21<<CNSHIFT,
		//E0-E9
        E0 = 64|NO_CN<<CNSHIFT, E1 = 65|NO_CN<<CNSHIFT,
        E2 = 66|NO_CN<<CNSHIFT, E3 = 67|NO_CN<<CNSHIFT,
        E4 = 68|NO_CN<<CNSHIFT, E5 = 69|NO_CN<<CNSHIFT,
        E6 = 70|NO_CN<<CNSHIFT, E7 = 71|NO_CN<<CNSHIFT,
        E8 = 72|NO_CN<<CNSHIFT, E9 = 73|NO_CN<<CNSHIFT,
        /*no E10 = 74, E11 = 75,
        E12 = 76, E13 = 77,
        E14 = 78, E15 = 79,*/
		//F0-F13
        F0 = 80|NO_CN<<CNSHIFT, F1 = 81|NO_CN<<CNSHIFT,
        F2 = 82|NO_CN<<CNSHIFT, F3 = 83|NO_CN<<CNSHIFT,
        F4 = 84|17<<CNSHIFT, F5 = 85|18<<CNSHIFT,
        /*no F6 = 86, F7 = 87,*/
        F8 = 88|NO_CN<<CNSHIFT, /*F9 = 89,
        F10 = 90, F11 = 91,*/
        F12 = 92|NO_CN<<CNSHIFT, F13 = 93|NO_CN<<CNSHIFT,
        /*no F14 = 94, F15 = 95,*/
		//G0-G13
        G0 = 96|NO_CN<<CNSHIFT, G1 = 97|NO_CN<<CNSHIFT,
        G2 = 98|NO_CN<<CNSHIFT, G3 = 99|NO_CN<<CNSHIFT,
        /*G4 = 100, G5 = 101,*/
        G6 = 102|8<<CNSHIFT, G7 = 103|9<<CNSHIFT,
        G8 = 104|10<<CNSHIFT, G9 = 105|11<<CNSHIFT,
        /* no G10 = 106, G11 = 107,*/
        G12 = 108|NO_CN<<CNSHIFT, G13 = 109|NO_CN<<CNSHIFT,
        G14 = 110|NO_CN<<CNSHIFT, G15 = 111|NO_CN<<CNSHIFT,

        //ANn 0-19
        AN0 = B0, AN1 = B1, AN2 = B2, AN3 = B3, AN4 = B4, 
		AN5 = B5, AN6 = B6, AN7 = B7, AN8 = B8, AN9 = B9, 
		AN10 = B10, AN11 = B11, AN12 = B12, AN13 = B13, 
		AN14 = B14, AN15 = B15,
		
		//CNn 0-21
		CN0 = C14, CN1 = C13, CN2 = B0, CN3 = B1, CN4 = B2, 
		CN5 = B3, CN6 = B4, CN7 = B5, CN8 = G6, CN9 = G7, 
		CN10 = G8, CN11 = G9, CN12 = B15, CN13 = D4, CN14 = D5, 
		CN15 = D6, CN16 = D7, CN17 = F4, CN18 = F5, CN19 = D13, 
		CN20 = D14, CN21 = D15,

        //PINn 1-64
        PIN1 = G15, /*PIN2 = Vdd,*/ PIN3 = E5, PIN4 = E6, PIN5 = E7,
        PIN6 = C1, PIN7 = C2, PIN8 = C3, PIN9 = C4, PIN10 = G6,
        PIN11 = G7, PIN12 = G8, /*PIN13 = MCLR,*/ PIN14 = G9, /*PIN15 = Vss, 
		PIN16 = Vdd,*/ PIN17 = A0, PIN18 = E8, PIN19 = E9, PIN20 = B5,
		PIN21 = B4, PIN22 = B3, PIN23 = B2, PIN24 = B1, PIN25 = B0, 
		PIN26 = B6, PIN27 = B7, PIN28 = A9, PIN29 = A10, /*PIN30 = AVdd, 
		PIN31 = AVss,*/ PIN32 = B8, PIN33 = B9, PIN34 = B10, PIN35 = B11,
		/*PIN36 = Vss, PIN37 = Vdd,*/ PIN38 = A1, PIN39 = F13, PIN40 = F12, 
		PIN41 = B12, PIN42 = B13, PIN43 = B14, PIN44 = B15, /*PIN45 = Vss, 
		PIN46 = Vdd,*/ PIN47 = D14, PIN48 = D15, PIN49 = F4, PIN50 = F5, 
		PIN51 = F3, PIN52 = F2, PIN53 = F8, /*PIN54 = VBUS, PIN55 = Vusb3v3,*/ 
		PIN56 = G3, PIN57 = G2, PIN58 = A2, PIN59 = A3, PIN60 = A4, 
		PIN61 = A5, /*PIN62 = Vdd,*/ PIN63 = C12, PIN64 = C15, /*PIN65 = Vss*/
		PIN66 = A14, PIN67 = A15, PIN68 = D8, PIN69 = D9, PIN70 = D10,
		PIN71 = D11, PIN72 = D0, PIN73 = C13, PIN74 = C14, /*PIN75 = Vss,*/
		PIN76 = D1, PIN77 = D2, PIN78 = D3, PIN79 = D12, PIN80 = D13,
		PIN81 = D4, PIN82 = D5, PIN83 = D6, PIN84 = D7, /*PIN85 = Vcap,
		PIN86 = Vdd,*/ PIN87 = F0, PIN88 = F1, PIN89 = G1, PIN90 = G0,
		PIN91 = A6, PIN92 = A7, PIN93 = E0, PIN94 = E1, PIN95 = G14, 
		PIN96 = G12, PIN97 = G13, PIN98 = E2, PIN99 = E3, PIN100 = E4
    };

    enum IOMODE : uint8_t {
        AIN = 0,
        IN = 1, INPU = 1<<3|1<<2|IN, INPD = 1<<4|IN, INL = 1<<2|IN,
        OUT = 2, OUTL = 1<<2|OUT
    };
    //constructor
    Pins(RPN, IOMODE = AIN);

    //r/w pins
    bool        pinval          () const;
    bool        latval          () const;
    void        latval          (bool) const;
    uint16_t    adcval          () const;
    void        low             () const;
    void        high            () const;
    void        invert          () const;
    void        on              () const;
    void        off             () const;
    bool        ison            () const;

    //pin modes
    void        lowison         (bool);
    void        digital_in      () const;
    void        analog_in       () const;
    void        digital_out     () const;
    void        odrain          (bool) const;
    void        pullup          (bool) const;
    void        pulldn          (bool) const;

    //icn
    void        icn             (bool) const;
    void        icn_rising      () const;
    void        icn_falling     () const;
    void        icn_risefall    () const;
    void        icn_mismatch    () const;
    bool        icn_flag        () const;
    bool        icn_stat        () const;
    void        icn_flagclr     () const;


    //get adc #
    uint8_t     an_num          ();
	//get icn #
	uint8_t		cn_num			();

    private:

    volatile uint32_t*  m_pt;       //base address
    const uint16_t      m_pn;       //pin mask
    bool                m_lowison;  //pin on val is low
    const uint8_t       m_an;       //ANn value
	const uint8_t		m_cn;		//CNn value

};
