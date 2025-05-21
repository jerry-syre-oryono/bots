RSI Expert Advisor (ea rsi.mq5) – Documentation
1. Overview
This Expert Advisor (EA) executes trades based on the Relative Strength Index (RSI) indicator. It opens a buy when RSI exits the overbought region (above 70 by default) and a sell when RSI exits the oversold region (below 30 by default).

The EA includes customizable parameters for RSI period, RSI level, lot size, stop loss, take profit, and trade management (closing positions on signal reversal).

2. Inputs
Parameter Name	Type	Description
InpMagicnumber	long	Unique identifier for this EA’s trades. Prevents interference with other EAs.
InpLotSize	double	Fixed lot size for each order. Must be >0 and ≤10.
InpRSIPeriod	int	Number of periods to calculate RSI. Must be >1.
InpRSILevel	int	RSI threshold level (e.g., 70). Used to detect overbought/oversold conditions.
InpStopLoss	int	Stop loss in points. 0 disables SL.
InpTakeProfit	int	Take profit in points. 0 disables TP.
InpCloseSignal	bool	If true, closes opposite-position trades when a new signal is generated.

3. Initialization (OnInit)
Validates all input parameters.

Sets up the RSI indicator handle using iRSI().

Configures the trade object to use the defined InpMagicnumber.

Prepares the RSI buffer to be accessed in series mode (latest value is index 0).

4. Tick Logic (OnTick)
Retrieves the current market tick using SymbolInfoTick().

Copies the last two RSI values using CopyBuffer():

buffer[0]: current RSI

buffer[1]: previous RSI

Displays RSI values in the chart using Comment().

Buy Conditions
No current buy position (cntBuy == 0).

RSI has crossed below the upper bound (from overbought): buffer[1] ≥ (100 - InpRSILevel) and buffer[0] < (100 - InpRSILevel).

Optional: closes all sell positions if InpCloseSignal = true.

Places a buy order with SL and TP (if defined).

Sell Conditions
No current sell position (cntSell == 0).

RSI has crossed above the lower bound (from oversold): buffer[1] ≤ InpRSILevel and buffer[0] > InpRSILevel.

Optional: closes all buy positions if InpCloseSignal = true.

Places a sell order with SL and TP (if defined).

5. Position and Risk Management Functions
CountOpenPositions(int &cntBuy, int &cntSell)
Iterates over all open positions.

Counts buy and sell positions that match the EA’s magic number.

Returns true if successful, false otherwise.

NormalisePrice(double &price)
Ensures the given price aligns with the market’s tick size.

Uses MathRound() to match valid price increments.

Returns true if successful.

ClosePositions(int all_buy_sell)
Closes positions based on the value of all_buy_sell:

1 = close buy positions only

2 = close sell positions only

Filters trades using the EA’s magic number.

Uses CTrade::PositionClose() to execute closure.

Prints errors if a position fails to close.

6. Deinitialization (OnDeinit)
Releases the RSI indicator handle to free system resources.

7. Trading Logic Diagram (Simplified)
pgsql
Copy
Edit
               +------------------+
               |   On each tick   |
               +--------+---------+
                        |
                        v
         +----------------------------+
         |  Get RSI buffer [0] and [1]|
         +----------------------------+
                        |
         +-------------------------------+
         | Count open Buy/Sell positions |
         +-------------------------------+
                        |
         |                     |
   [BUY Conditions]     [SELL Conditions]
         |                     |
         v                     v
  Open Buy order         Open Sell order
         |                     |
[Optional: Close Sell] [Optional: Close Buy]
8. RSI Signal Logic Summary
Trade Type	Condition
Buy	RSI exits overbought region: RSI_prev ≥ 100 - level and RSI_curr < 100 - level
Sell	RSI exits oversold region: RSI_prev ≤ level and RSI_curr > level
