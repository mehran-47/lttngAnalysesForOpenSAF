elasticity_engine_main
    loop1: 
        switchcall(SI_name, SG-name, elasticity_action_type, elasticityActionSelectorOnSInumber, elasticityActionType, IMMhandle, argumentsFromCommandLine)
        elasticityT: 1 is for single SI; 2 is for multiple SIs
        elasticityact: 1 is increase; 2 is for decrease


single_SIincreaseNwayAct (11675 - 12626)
    loop2:
        if(noassgSUs <= noSIassg) is 'true' in 'single_SIincreaseNwayAct'
        increase saAmfSGNumPrefAssignedSU 

    loop3:
        decrease saAmfSGMaxActiveSIsperSU 

    loop4:
        check if need to increase saAmfSGMaxActiveSIsperSU (comes before loop2 and loop3)

    loop6:
        the beginning of 'single_SIincreaseNwayAct' after initializing all variables

    
multiple_SIincreaseNwayAct (6291-6729)
    loop7:
        the beginning of 'multiple_SIincreaseNwayAct' after initializing all variables

single_SIdecreaseNwayAct (6733-7622)
    loop8:
        the beginning of 'single_SIdecreaseNwayAct' after initializing all variables

    loop9:
        decrease 'saAmfSGNumPrefAssignedSUs'

    loop10:
        increase activeSIsperSU

    loop11:
        if(curr_assg_SUs==0) reset IMM handle

single_SIIncreaseNPM (8194 - 10053)
    loop12:
        decrease saAmfSGMaxActiveSIsperSU

    loop13:
        if(noassgSUs < no_SIs ), need to increase number of SUs

    loop14:
        adjust the number of standby SUs by decreasing saAmfSGMaxStandbySIsperSU

    loop15:
        try to change SU rank first, then try to change cluster level


single_SIDecreaseNPM (11675 - 12623)
    loop16:
        decrease saAmfSGNumPrefStandbySUs

    
single_SIIncreaseNway (12628 - 14674)
    loop17:
        decrease saAmfSGMaxActiveSIsperSU

    loop18:
        decrease saAmfSGMaxStandbySIsperSU

    loop20:
        decrease the saAmfSGMaxStandbySIsperSU

    loop21:
        decrease saAmfSGMaxActiveSIsperSU

multiple_SIIncreaseNway (14679 - 15583)
    loop22:
        the beginning of 'multiple_SIIncreaseNway' after initializing all variables

    loop23:
        checking/increase saAmfSGMaxStandbySIsperSU


single_SIDecreaseNway
    loop24:
         the beginning of 'single_SIDecreaseNway' after initializing all variables
    
multiple_SIIncreaseNPM (10061 - 11674)
    loop25:
        the beginning of 'multiple_SIIncreaseNPM' after initializing all variables

    loop26:
        decrease saAmfSGMaxStandbySIsperSU

    loop27:
        increase saAmfSGNumPrefStandbySUs

    loop28:
        increase saAmfSGNumPrefStandbySUs

    loop29:
        decrease saAmfSGNumPrefStandbySUs