#include "Mapper.h"

using namespace std;

static bool IsConnected(int * maparray, int mapw, int maph, int * gapcount);
static bool IsReachable(int * maparray, int mapw, int maph, int dexfrom, int dexto);
static bool ArrayContains(int * list, int listsize, int findnum);
static void MakeWhole(int * maparray, int mapw, int maph);

int * Map::Init()
{
	////cout << "Hi";
	int * maparray;
	//int arraysize = 100;

	int mapw = 10;////<<<<-----------------------------------------------------width
	int maph = 10;////<<<<-----------------------------------------------------height
	int arraysize = maph * mapw;
	int xspeed = 2;
	int jumpstartspeed = 2;
	int gravity = 1;//it gets negative later

	arraysize = maph * mapw;


	maparray = (int*)calloc(arraysize, sizeof(int));

	for(int c = 0; c < arraysize; c++)
	{
		if(c < (arraysize - 2 * mapw) )
		{
			maparray[c] = 0;
		}
		else
		{
			maparray[c] = 1;
		}
	}

	//none = 0
	//ground = 1
	//rightEdge = 2
	//leftEdge = 3
	//coin = 7
	//start = 8
	//goal = 9
	//
	//1. Must be able to get to goal
	//2. Coins above surfaces
	//3. must be able to jump over any gaps--or unreachable spots
	//4. goal must be away from user
	//
	//

	//1.Generate Base
	//2.Add to other platforms
	//3.Choose a start point
	//3.Delete ledges--check after each deletion everywhere is reachable
	//4.Add coins and goal
	//
	int ledgepermille = 120;//170;////<<<<--------------------------------------------changing number effects platforms
	srand(time(0));
	for(int d = arraysize - 1 - mapw; d >= mapw; d--)//dont populate top row//and also dont go to the start of a row
	{
		if(d % mapw == 0)
		{
			d--;//new
		}
		if( (rand() % 1000) < ledgepermille)
		{
			maparray[d] = 1;
			maparray[d - 1] = 1;
		}
	}

	int gappermille = 90;//40;
	//srand(time(0));
	int gapcount = 0;
	for(int s = 0; s < mapw; s++)
	{
		if( (rand() % 1000) < gappermille)////<<<<--------------------------------------------changing number effects gaps
		{
			maparray[ (mapw * ( maph - 2 )) + s] = 0;
			maparray[ (mapw * ( maph - 1 )) + s] = 0;
			////if(s >) 
			gapcount++;
		}
	}

	while(!IsConnected(maparray, mapw, maph, &gapcount) )
	{

	}

	MakeWhole(maparray, mapw, maph);

	//show(cout)
	for(int i = 0; i < arraysize; i++)
	{
		if( (i % mapw) == mapw - 1 )
		{
			cout << maparray[i] << endl;
		}
		else
		{
			cout << maparray[i];
		}
	}

	return maparray;
}


//
//
//this deals with platforms to and from, coins, starts, goals, need to measure the below value
static bool IsConnected(int * maparray, int mapw, int maph, int * gapcount)
{
	bool connected = true;
	int xspeed = 2;
	int jumpstartspeed = 2;
	int gravity = 1;//it gets negative later
	int arraysize = maph * mapw;

	int * ledgexlist;
	int * ledgeylist;
	int * ledgedexlist;
	int * canreachdexlist;

	//ledgexlist = (int*)calloc(
	int platcounter = 0;

	////for(int f = 0; f < mapw - 1; f++)//this might get complicated
	int indexer = mapw * (maph - 1);
	
	while(indexer != mapw - 1 )
	{
	//maparray[indexer] = 5;
		if(maparray[indexer] != 0 && indexer >= mapw && maparray[indexer - mapw] == 0)
		{
			platcounter++;
		}

		if(indexer >= mapw)
		{
			indexer = indexer - mapw;
		}
		else
		{
			indexer = mapw * (maph - 1) + indexer + 1;
			//maparray[indexer] = 6;
		}
	}

	ledgexlist = (int*)calloc(platcounter,sizeof(int));
	ledgeylist = (int*)calloc(platcounter,sizeof(int));
	ledgedexlist = (int*)calloc(platcounter,sizeof(int));
	canreachdexlist = (int*)calloc(platcounter,sizeof(int));

	int canreachtot = 0;
	for(int r = 0; r < platcounter; r++)
	{
		canreachdexlist[r] = -1;
	}


	indexer = mapw * (maph - 1);
	int onplat = 0;
	while(indexer != mapw - 1 )
	{
	//maparray[indexer] = 5;
		if(maparray[indexer] != 0 && indexer >= mapw && maparray[indexer - mapw] == 0)
		{
			//ledgexlist = ;
			//ledgeylist = ;
			ledgedexlist[onplat] = indexer;
			onplat++;
		}

		if(indexer >= mapw)
		{
			indexer = indexer - mapw;
		}
		else
		{
			indexer = mapw * (maph - 1) + indexer + 1;
			//maparray[indexer] = 6;
		}
	}

	cout << endl << platcounter << endl;

	int startpos = rand() % platcounter;
	maparray[ledgedexlist[startpos] - mapw] = 8;////

	int reachables = 0;

	canreachdexlist[reachables] = ledgedexlist[startpos];//startpos;
	reachables++;
	//
	//go through each reachable, then search each platformledge
	cout << "Checking all leges:" << endl;
	for(int e = 0; e < reachables; e++)
	{
		for(int q= 0; q < platcounter; q++)
		{
			if( !ArrayContains(canreachdexlist, reachables, ledgedexlist[q]) )
			{
				if(  (ledgedexlist[q] % mapw != 0) && (ledgedexlist[q] == canreachdexlist[e] + 1) )
				{
					canreachdexlist[reachables] = ledgedexlist[q];
					//cout << "go " << canreachdexlist[e] << " to "  << ledgedexlist[q] << endl;
					reachables++;
				}
				else if((canreachdexlist[e] % mapw != 0) && (ledgedexlist[q] + 1 == canreachdexlist[e]) )
				{
					//cout << "go " << canreachdexlist[e] << " to "  << ledgedexlist[q] << endl;
					canreachdexlist[reachables] = ledgedexlist[q];
					reachables++;
				}
				else
				{
					if(IsReachable(maparray, mapw, maph, canreachdexlist[e], ledgedexlist[q]) )
					{
						canreachdexlist[reachables] = ledgedexlist[q];
						//cout << "go " << canreachdexlist[e] << " to "  << ledgedexlist[q] << endl;
						reachables++;
					}
				}
			}
		}
	}

	if(reachables == platcounter)
	{
		connected = true;
		cout << "platforms: ";
		for(int i = 0; i < platcounter; i++)
		{
			cout << " " << ledgedexlist[i] << " ";
		}
		cout << endl;
		//set coins, goal
		//
		//
		bool gateset = false;
		int randnum = 0;
		while(!gateset)
		{
		for(int b = 0; b < platcounter; b++)
		{
			//cout << " " << ledgedexlist[i] << " ";
			if(maparray[ledgedexlist[b] - mapw] != 8)
			{
				randnum = rand() % 10;////<<<<-------------------------------------------will effect coins
				if(!gateset && randnum == 0)
				{
					if( ( maparray[ledgedexlist[b] - 1 - mapw] != 8 ||  ledgedexlist[b] % mapw == 0 ) && ( maparray[ledgedexlist[b] + 1 - mapw] != 8 ||  (ledgedexlist[b] % mapw) == (mapw - 1) ) )
					{
						maparray[ledgedexlist[b] - mapw] = 9;
						gateset = true;
					}
				}
				else
				{
					randnum = rand() % 3;////<<<<----------------------------------------will effect coins
					if(randnum == 0)
					{
						maparray[ledgedexlist[b] - mapw] = 7;
					}
				}
			}
		}
		}

		//
	}
	else
	{
		connected = false;//delete an unreachable spot?
		maparray[ledgedexlist[startpos] - mapw] = 0;
		if(*gapcount > 0)
		{
			int flip = rand() % 8;////<<<<--------------------------------------------changing number effects platforms
			if(flip == 0)
			{
				for(int r = 0; r < mapw; r++)
				{
					if( maparray[ (mapw * ( maph - 2 )) + r] == 0 )
					{
						maparray[ (mapw * ( maph - 2 )) + r] = 1;
						maparray[ (mapw * ( maph - 1 )) + r] = 1;
						r = mapw;
					}
				}
				*gapcount--;
			}
			else if(flip > 5)////<<<<--------------------------------------------changing number effects platforms
			{
				for(int w = 0; w < platcounter; w++)
				{
					if(!ArrayContains(canreachdexlist, reachables, ledgedexlist[w]) && ledgedexlist[w] < ( (maph - 2) * mapw) )
					{
						//////////this is new section v
						/*////
						if(ledgedexlist[w] % mapw != 0)//split or destroy//if not on leftborder _g
						{
							if(ledgedexlist[w] % mapw != (mapw - 1))//if not on rightborder _g_
							{
								if(maparray[ledgedexlist[w] + 1] == 1 && (ledgedexlist[w] + 1) % mapw != (mapw - 1) )//if right1 ground and not rborder _gg_
								{
									if(maparray[ledgedexlist[w] + 2] == 1)//if right2 ground
									{
										if(maparray[ledgedexlist[w] - 1] != 1)//if left NOT ground
										{
										maparray[ledgedexlist[w]] = 0;
										}
										else if( (ledgedexlist[w] - 1) % mapw != 0 && maparray[ledgedexlist[w] - 2] == 1)//five in row
										{
											maparray[ledgedexlist[w]] = 0;
										}
										else
										{
											if( (ledgedexlist[w] - 1) % mapw == 0 )//four in row, left border
											{
												if(maparray[ledgedexlist[w] + 3] == 1)//five in row, left border after -1
												{
													maparray[ledgedexlist[w] + 1] = 0;
												}
												else//four in row, left border
												{
													maparray[ledgedexlist[w] + 2] = 0;
												}
											}
											else//four in row, no border
											{
											maparray[ledgedexlist[w]] = 0;
											maparray[ledgedexlist[w] + 1] = 0;
											maparray[ledgedexlist[w] + 2] = 0;
											maparray[ledgedexlist[w] - 1] = 0;
											}
										}
									}
									else
									{
										if(maparray[ledgedexlist[w - 1]] == 1)
										{

										}
									}
								}
								else if(maparray[ledgedexlist[w] + 1] == 1)
								{
									if(maparray[ledgedexlist[w] - 1] == 1)
									{
										maparray[ledgedexlist[w ]+ 1] = 0;
									}
									else
									{

									}
								}
								else
								{
								}
							}
							else
							{

							}
						}
						else
						{

						}
						if(maparray[ledgedexlist[w]])*/
						/////////this was new section ^

						int neilist[] = {-2,-2,-2,-2,-2};//-1 is at or after boundary
						int neicount = 0;
						for(int u = 0; u < 5; u++)
						{
							if( ((ledgedexlist[w] + u - 2) / mapw) != (ledgedexlist[w] / mapw) )
							{
								neilist[u] = -1;
							}
							else
							{
								neilist[u] = maparray[ledgedexlist[w] + u - 2];
								if(maparray[ledgedexlist[w] + u - 2])
								{
									neicount++;
								}
							}
						}
						if(neicount == 5 || neicount == 1)
						{
							maparray[ledgedexlist[w]] = 0;
						}
						if(neicount == 2)
						{
							if(neilist[1] == 1)
							{
								maparray[ledgedexlist[w]] = 0;
								maparray[ledgedexlist[w] - 1] = 0;
							}
							else
							{
								maparray[ledgedexlist[w]] = 0;
								maparray[ledgedexlist[w] + 1] = 0;
							}
						}
						if(neicount == 3)
						{
							if(neilist[1] == 1 && neilist[3] == 1)
							{
								maparray[ledgedexlist[w]] = 0;
								maparray[ledgedexlist[w] + 1] = 0;
								maparray[ledgedexlist[w] - 1] = 0;
							}
							else
							{
								maparray[ledgedexlist[w]] = 0;
							}
						}
						if(neicount == 4)
						{
							if(neilist[0] != 1)
							{
								maparray[ledgedexlist[w]] = 0;
								maparray[ledgedexlist[w] + 1] = 0;
								maparray[ledgedexlist[w] - 1] = 0;
								maparray[ledgedexlist[w] + 2] = 0;
							}
							else
							{
								maparray[ledgedexlist[w]] = 0;
								maparray[ledgedexlist[w] + 1] = 0;
								maparray[ledgedexlist[w] - 1] = 0;
								maparray[ledgedexlist[w] - 2] = 0;
							}
						}

					////maparray[ledgedexlist[w]] = 0;
					w = platcounter;
					}
				}
			}
		}
		else
		{
			for(int w = 0; w < platcounter; w++)
			{
				if(!ArrayContains(canreachdexlist, reachables, ledgedexlist[w]) && ledgedexlist[w] < ( (maph - 2) * mapw) )
				{
					int flip = rand() % 3;////<<<<--------------------------------------------changing number effects platforms
					if(flip == 0)
					{///////////////////////////////////////////////////////////////////////////////////
						int neilist[] = {-2,-2,-2,-2,-2};//-1 is at or after boundary
						int neicount = 0;
						for(int u = 0; u < 5; u++)
						{
							if( ((ledgedexlist[w] + u - 2) / mapw) != (ledgedexlist[w] / mapw) )
							{
								neilist[u] = -1;
							}
							else
							{
								neilist[u] = maparray[ledgedexlist[w] + u - 2];
								if(maparray[ledgedexlist[w] + u - 2])
								{
									neicount++;
								}
							}
						}
						if(neicount == 5 || neicount == 1)
						{
							maparray[ledgedexlist[w]] = 0;
						}
						if(neicount == 2)
						{
							if(neilist[1] == 1)
							{
								maparray[ledgedexlist[w]] = 0;
								maparray[ledgedexlist[w] - 1] = 0;
							}
							else
							{
								maparray[ledgedexlist[w]] = 0;
								maparray[ledgedexlist[w] + 1] = 0;
							}
						}
						if(neicount == 3)
						{
							if(neilist[1] == 1 && neilist[3] == 1)
							{
								maparray[ledgedexlist[w]] = 0;
								maparray[ledgedexlist[w] + 1] = 0;
								maparray[ledgedexlist[w] - 1] = 0;
							}
							else
							{
								maparray[ledgedexlist[w]] = 0;
							}
						}
						if(neicount == 4)
						{
							if(neilist[0] != 1)
							{
								maparray[ledgedexlist[w]] = 0;
								maparray[ledgedexlist[w] + 1] = 0;
								maparray[ledgedexlist[w] - 1] = 0;
								maparray[ledgedexlist[w] + 2] = 0;
							}
							else
							{
								maparray[ledgedexlist[w]] = 0;
								maparray[ledgedexlist[w] + 1] = 0;
								maparray[ledgedexlist[w] - 1] = 0;
								maparray[ledgedexlist[w] - 2] = 0;
							}
						}
					}//////////////////////////////////////////////////////////////////////////////
					//maparray[ledgedexlist[w]] = 0;
					w = platcounter;
				}
			}
		}

	}

	return connected;
}


static bool ArrayContains(int * list, int listsize, int findnum)
{
	bool found = false;

	for(int c = 0; c < listsize; c++)
	{
		if(list[c] == findnum)
		{
			found = true;
		}
	}

	return found;
}

//
//
//this deals with platforms to and from, coins, starts, goals, need to measure the below value
static bool IsReachable(int * maparray, int mapw, int maph, int dexfrom, int dexto)
{
	double xspeed = 1;////<<<<-----------------------------------------------------------
	double jumpstartspeed = 2;////<<<<---------------------------------------------------
	double gravity = 1;//it gets negative later////<<<<----------------------------------
	bool reachable = false;

	int fromx = dexfrom % mapw;
	int fromy = dexfrom / maph;
	int tox = dexto % mapw;
	int toy = dexto / maph;
	double diffx = abs(fromx - tox); 
	double diffy =  abs(fromy - toy);


	////cout << jumpstartspeed * (diffx/xspeed) + (.5 * -1 * gravity)*(diffx/xspeed)*(diffx/xspeed) << " " << diffx << endl;
	if( fromy > toy )//if lower to higher
	{
				//speed on jumpstart  
		if(//diffy < jumpstartspeed + (diffx/xspeed * gravity) +
			diffy < jumpstartspeed * (diffx/xspeed) + (.5 * -1 * gravity)*(diffx/xspeed)*(diffx/xspeed)
			
			)
		{
			if(diffx == 0)
			{

			}
			else if(tox > fromx)//if going right
			{
				if( (maparray[dexto - mapw - 1] == 8 || maparray[dexto - 1] == 0) && (maparray[dexto - mapw - 1] == 0 || maparray[dexto - mapw - 1] == 8))
				{
					reachable = true;//???
				}
			}
			else////if going left
			{
				if( (maparray[dexto + 1] == 8 || maparray[dexto + 1] == 0) && (maparray[dexto - mapw + 1] == 8 || maparray[dexto - mapw + 1] == 0) )
				{
					reachable = true;//???
				}
			}



		}
	}
	else// if higher to lower
	{
		if(//diffy < jumpstartspeed + (diffx/xspeed * gravity) +
			(-1) * diffy < jumpstartspeed * (diffx/xspeed) + (.5 * -1 * gravity)*(diffx/xspeed)*(diffx/xspeed)
			
			)
		{
			if(diffx == 0)
			{

			}
			else if(diffx > 1)
			{
				if(tox > fromx)
				{
					if(maparray[dexto - mapw - 1] != 1)
					{
						reachable = true;
					}
				}
				else
				{
					if(maparray[dexto - mapw + 1] != 1)
					{
						reachable = true;
					}
				}
			}
			else
			{
				if(tox > fromx)
				{
					if(maparray[dexto - mapw - 1] != 1 && maparray[dexto - mapw - mapw - 1] != 1)
					{
						reachable = true;
					}
				}
				else
				{
					if(maparray[dexto - mapw + 1] != 1 && maparray[dexto - mapw - mapw + 1] != 1)
					{
						reachable = true;
					}
				}
				//reachable = true;
				////cout << "go " << dexfrom << " to "  << dexto << endl;
			}
		}
	}

	if(reachable)
	{
		cout << "go " << dexfrom << " to "  << dexto << endl;
	}

	return reachable;
}

static void MakeWhole(int * maparray, int mapw, int maph)
{
	int arraysize = mapw * maph;

	for(int c = 0; c < arraysize; c++)
	{
		if(maparray[c] == 1)
		{
		if(c % mapw != 0)
		{
			if(maparray[c - 1] == 0)
			{
				maparray[c] = 3;//is a left edge
			}
			else if(c % mapw != (mapw - 1) && maparray[c + 1] == 0)
			{
				maparray[c] = 2;//is a right edge
			}
		}
		else
		{
			if(c % mapw != (mapw - 1) && c % mapw != 0 )
			{
				maparray[c] = 2;//is a right edge
			}
		}
		}
	}
}
