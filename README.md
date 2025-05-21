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





📘 Breakout EA v1 – Documentation
✅ Overview
Name: breakout ea.1.mq5
Type: Automated Expert Advisor (EA)
Platform: MetaTrader 5 (MQL5)
Strategy: Range Breakout with Optional Martingale and Risk Management
Author: MetaQuotes Ltd. / Modified by User
Created: 2024

🎯 Strategy Description
This EA identifies price consolidation zones on a specified timeframe. It then enters trades when the price breaks above or below these zones, targeting continuation. A martingale-based position recovery system and risk-based lot sizing are implemented. Visual rectangles are drawn to show detected breakout ranges.

⚙️ Input Parameters
Name	Type	Description
RiskPercent	double	Percentage of balance risked per trade (e.g., 0.5 = 0.5%)
MartingaleFactor	double	Multiplier applied to lot size when recovering from losses
TpPercent	double	Take-profit target as a percentage of balance
Timeframe	ENUM_TIMEFRAMES	Timeframe to analyze (e.g., PERIOD_H1)
MinRangeBars	int	Number of bars used to define the tight price range
ComparisonBars	int	Bars used to compare if current range is relatively small
MinRangeFactor	double	Current range must be smaller than (comparison range × factor) to be considered consolidation

🧠 Key Variables
CSetup class: Stores breakout setup state.

time1, timex: Timestamps of range start/end.

high, low: Price range boundaries.

posTicket: Current trade ticket ID.

lostMoney: Cumulative losses for this setup.

CTrade trade: MQL5 trade execution object.

📈 Core Logic
1. Detect Consolidation
Finds highest and lowest prices over MinRangeBars.

Compares current range to previous range over ComparisonBars.

If range is small enough (rangeBarsSize < comparisonBarsSize * MinRangeFactor):

Store the breakout zone (high, low, time1, timex).

Draw a rectangle for visualization.

2. Entry Trigger
If price breaks above high, attempt a buy.

If price breaks below low, attempt a sell.

3. Martingale Logic
If previous position was in opposite direction and closed at a loss:

Multiply last lot size by MartingaleFactor to recover loss.

4. Lot Calculation
Based on RiskPercent of balance.

Risk per lot is derived from tick size and range width.

5. Exit Condition
When floating + recovered loss > TpPercent of balance:

Close position.

Reset the setup (posTicket = 0, lostMoney = 0, time1 = 0).

🧾 Object Drawing
Rectangle is drawn to visualize the breakout zone:

mql5
Copy
Edit
ObjectCreate(0, objName, OBJ_RECTANGLE, 0, time1, high, timex, low);
⚠️ Known Issues
Issue	Description
Lot size bug	Inverse logic used in lot calculation (fixed in suggestions).
Martingale lot logic	Adds MartingaleFactor instead of multiplying (should be fixed).
Position read	Reads from PositionGet... without ensuring PositionSelectByTicket() returns true.
Retcode not verified	Trade success (trade.ResultRetcode()) not checked after order placement.