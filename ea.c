//+------------------------------------------------------------------+
//|                                                       ea rsi.mq5 |
//|                                  Copyright 2024, MetaQuotes Ltd. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+
#property copyright "Copyright 2024, MetaQuotes Ltd."
#property link      "https://www.mql5.com"
#property version   "1.00"

//+------------------------------------------------------------------+
//| include                                |
//+------------------------------------------------------------------+
#include <Trade\Trade.mqh>

//+------------------------------------------------------------------+
//| Inputs
//+------------------------------------------------------------------
static input long    InpMagicnumber = 546812;  //magic number
static input double  InpLotSize = 1.00;        //lot size
input int            InpRSIPeriod = 14;        //rsi period    
input int            InpRSILevel  =70;         //resi level upper
input int            InpStopLoss  = 200;       //stop loss in points (0=off)
input int            InpTakeProfit = 100;      //take profit in points (0=off)
input bool           InpCloseSignal = false;   //close tradees by opposite signal


//+------------------------------------------------------------------+
//| Global variables                                  |
//+------------------------------------------------------------------+
int handle ;
double buffer[];
MqlTick currentTick;
CTrade trade;
datetime openTimeBuy = 0;
datetime openTimeSell = 0;
 
 
int OnInit()
  {
   
   //check user inputs 
   
   if(InpMagicnumber<=0){
         Alert("magicnumber <= 0");
         return INIT_PARAMETERS_INCORRECT;
   }
   if(InpLotSize<=0 || InpLotSize>10){
         Alert("lotsize <= 0 or >10");
         return INIT_PARAMETERS_INCORRECT;
   }
   if(InpRSIPeriod<=1){
         Alert("RSI period <= 1");
         return INIT_PARAMETERS_INCORRECT;
   }
   if(InpRSILevel>=100 || InpRSILevel<=50){
         Alert("rsi level >= or <=50");
         return INIT_PARAMETERS_INCORRECT;
   }
   if(InpStopLoss<0){
         Alert("stoploss < 0");
         return INIT_PARAMETERS_INCORRECT;
   }
   if(InpTakeProfit<0){
         Alert("takeprofit < 0");
         return INIT_PARAMETERS_INCORRECT;
   }
   
   //set magic number to trade objejct 
   trade.SetExpertMagicNumber(InpMagicnumber);
   
   //create rsi handle 
   handle = iRSI(_Symbol,PERIOD_CURRENT,InpRSIPeriod,PRICE_CLOSE);
   if (handle == INVALID_HANDLE){
       Alert("failed to create indicator handle ");
       return INIT_FAILED;
   }
   
   //SET BUFFER AS SERIES IE SET THE ARRAY TO BE ACCESSED AS SERIES 
   ArraySetAsSeries(buffer,true);
   
   return(INIT_SUCCEEDED);
  }
  
  
//+------------------------------------------------------------------+
//| Expert deinitialization function                                 |
//+------------------------------------------------------------------+
void OnDeinit(const int reason)
  {

   //release indicator handle
   if (handle != INVALID_HANDLE){
       IndicatorRelease(handle);
   }
   
  }
//+------------------------------------------------------------------+
//| Expert tick function                                             |
//+------------------------------------------------------------------+
void OnTick()
  {

    //get current tick 
    if(!SymbolInfoTick(_Symbol,currentTick)){
       Print("failed to get current tick ");return;
    }
    
    //get rsi valuesb
    int values = CopyBuffer(handle,0,0,2,buffer);
    if(values != 2){
       Print ("failed to get indicator values");
       return;
    }
    
    Comment("buffer[0]:",buffer[0],
             "\n buffer[1]:",buffer[1]);
             
       
      //COUNT ALL OPEN POSITIONS 
      int cntBuy, cntSell;
      if(!CountOpenPositions(cntBuy,cntSell)){return;}
      
      //checking foor buy position active 
      if(cntBuy==0 && buffer[1]>=(100-InpRSILevel) && buffer[0]<(100-InpRSILevel) && openTimeBuy!=iTime(_Symbol,PERIOD_CURRENT,0)){
            
          openTimeBuy= iTime(_Symbol,PERIOD_CURRENT,0);
          if(InpCloseSignal){if(!ClosePositions(2)){return;}}
          double sl = InpStopLoss == 0 ? 0: currentTick.bid -InpStopLoss * _Point;
          double tp = InpTakeProfit == 0 ? 0:currentTick.bid + InpTakeProfit * _Point;
          
          if(!NormalisePrice(sl)){return;}
          if(!NormalisePrice(tp)){return;}
          
          trade.PositionOpen(_Symbol,ORDER_TYPE_BUY,InpLotSize,currentTick.ask,sl,tp,"rsi ea buy");
      
      }
      
       //checking foor sell position active 
      if(cntSell==0 && buffer[1]<=InpRSILevel && buffer[0]>InpRSILevel && openTimeSell!=iTime(_Symbol,PERIOD_CURRENT,0)){
            
          openTimeSell= iTime(_Symbol,PERIOD_CURRENT,0);
          if(InpCloseSignal){if(!ClosePositions(1)){return;}}
          double sl = InpStopLoss == 0 ? 0: currentTick.bid +InpStopLoss * _Point;
          double tp = InpTakeProfit == 0 ? 0:currentTick.bid - InpTakeProfit * _Point;
          
          if(!NormalisePrice(sl)){return;}
          if(!NormalisePrice(tp)){return;}
          
          trade.PositionOpen(_Symbol,ORDER_TYPE_SELL,InpLotSize,currentTick.bid,sl,tp,"rsi ea sell");
      
      }
      
      
   
  }
  
  //+------------------------------------------------------------------+
//| Custom user functions                                             |
//+------------------------------------------------------------------+

 //count open positions
  bool CountOpenPositions(int &cntBuy, int &cntSell){
      
      cntBuy = 0;
      cntSell =0;
      
      int total = PositionsTotal();
      for(int i=total-1; i>=0; i--){
          ulong ticket = PositionGetTicket(i);
          if (ticket<=0){Print("failed to get position ticket");return false;}
          if(!PositionSelectByTicket(ticket)){Print("failed to select position");return false;}
          long magic;
          
          if(!PositionGetInteger(POSITION_MAGIC,magic)){Print("failed to get position magic number");return false;}
          if(magic == InpMagicnumber){
             long type;
             if(!PositionGetInteger(POSITION_TYPE,type)){Print("failed to get position type");return false;}
             if(type == POSITION_TYPE_BUY){cntBuy++;}
             if(type == POSITION_TYPE_SELL){cntSell++;}
          
          }
      }
       
       return true;
  }
   
 //normalise the price 
 bool NormalisePrice(double &price){
      
      double tickSize = 0;
      if(!SymbolInfoDouble(_Symbol,SYMBOL_TRADE_TICK_SIZE,tickSize)){
          Print("failed to get tick size");
          return false;
      }
      price = NormalizeDouble(MathRound(price/tickSize)*tickSize,_Digits);
      
      return true;
 }
   
   
   //close positions
  bool ClosePositions(int all_buy_sell){
      
      
      int total = PositionsTotal();
      for(int i=total-1; i>=0; i--){
          ulong ticket = PositionGetTicket(i);
          if (ticket<=0){Print("failed to get position ticket");return false;}
          if(!PositionSelectByTicket(ticket)){Print("failed to select position");return false;}
          long magic;
          
          if(!PositionGetInteger(POSITION_MAGIC,magic)){Print("failed to get position magic number");return false;}
          if(magic == InpMagicnumber){
             long type;
             if(!PositionGetInteger(POSITION_TYPE,type)){Print("failed to get position type");return false;}
             if(all_buy_sell == 1 && type ==POSITION_TYPE_SELL){continue;}
             if(all_buy_sell == 2 && type ==POSITION_TYPE_BUY){continue;}
             trade.PositionClose(ticket);
             
             if(trade.ResultRetcode() != TRADE_RETCODE_DONE){
                Print("failed to close position. ticket:",
                 (string)ticket,"result:",(string)trade.ResultRetcode(),":",trade.CheckResultRetcodeDescription());
             }
          
          }
      }
       
       return true;
  }
//+------------------------------------------------------------------+
//END.............