int pMsg(POINT point, UINT uMsg, UINT wParam)
{
	short LMouseAct = GetAsyncKeyState(VK_LBUTTON);
	if((InArea(point)) && (Enable) && (Visible))
	{
		switch(uMsg)
		{
			case WM_MOUSEMOVE:
				if(LMouseAct)// && (Act == E_Drag || Act == E_LDown))
				{
					if(Dragable)
					{
						if(Act != E_Normal)
						{
							DragTo(point);
							return E_Drag; //Drag Act
						}
					}
					else
					{
							Act = E_LDown;
							return E_LDown; //Down Act
					}
				}
				else
				{
					if(Act != E_LDown)
					{
						Act = E_Over;
						return E_Over; //Move Act
					}
				}
				return E_Move;
																		
    		case WM_LBUTTONDOWN:
				Act = E_LDown;
				return E_LDown;
					
			case WM_RBUTTONDOWN:
				Act = E_LDown;
				return E_RDown;

			case WM_LBUTTONUP:
				Act = E_Over;
				return E_LUp;

			case WM_RBUTTONUP:
				Act = E_Over;
				return E_RUp;

		}
		return E_Over;
	}
	else if((Act == E_LDown) && (uMsg == WM_MOUSEMOVE) && (Dragable))//outside area but still drag
	{
	   DragTo(point);
	   return E_Drag;
	}
	else //outside area
	{
		Act = E_Normal;
		return E_Normal;
	}
}