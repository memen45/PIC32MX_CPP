
class Oc {
	enum OCX { OC1, OC2, OC3, OC4, OC5 };
	
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
		LOW_HIGH,
		HIGH_LOW,
		TOGGLE,
		LOW_SINGLE_PULSE,
		LOW_CONTIN_PULSE,
		PWM_NO_FAULT,
		PWM_FAULT
	};
	void			mode			(MODE);				//set oc mode
	
	volatile uint32_t* m_ocx_con;
}