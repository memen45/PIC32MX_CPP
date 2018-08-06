
#include <cstdint>

struct Oc {
	enum OCX { OC1, OC2, OC3, OC4, OC5 };
                    Oc              (OCX);
	//OCxcon
    void            on              (bool);             //oc on/off
	void			sidl			(bool);				//stop in idle mode
	
	enum CP_MODE : bool { CP16BITS, CP32BITS };
	void			cp_mode			(CP_MODE);				//16bit or 32bit timer
	
	bool			fault			();					//true if fault condition occured
	
	enum TMR_SEL : bool { TIMER_2, TIMER_3 };
	void			tmr_source		(TMR_SEL);			//timer source selection
	
	enum MODE : uint8_t { 
		DISABLE,
		HIGH,
		LOW,
		TOGGLE,
		SINGLE_PULSE,
		CONTIN_PULSE,
		PWM_NO_FAULT,
		PWM_FAULT
	};
	void			mode			(MODE);				//set oc mode
	
	//OCxr
	void			r_pri			(uint32_t);			//set primary compare register
	
	//OCxrs
	void			r_sec			(uint32_t);			//set secondary compare register
	
	volatile uint32_t* m_ocx_con;
};