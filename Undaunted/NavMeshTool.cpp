#include "NavmeshTool.h"
#include "SKSELink.h"
namespace Undaunted
{

	TileMap currentMap;

	std::string floattostring(float value)
	{
		char hex[9];
		sprintf(hex, "%08X", *(unsigned long int*) & value);
		//	_MESSAGE("float value: %s", hex);
		std::string output = "";
		output += hex[6];
		output += hex[7];
		output += ' ';
		output += hex[4];
		output += hex[5];
		output += ' ';
		output += hex[2];
		output += hex[3];
		output += ' ';
		output += hex[0];
		output += hex[1];
		return output;
	}

	std::string uint32tostring(UInt32 value)
	{
		char hex[9];
		sprintf(hex, "%04X", *(unsigned long int*) & value);
		std::string output = "";
		output += hex[2];
		output += hex[3];
		output += ' ';
		output += hex[0];
		output += hex[1];
		return output;
	}

	void AddVertex(Vert vertex)
	{
		_MESSAGE("verts := NewArrayElement(nvnm, 'Vertices');seev(nvnm, 'Vertices\\[%i]', '%s %s %s');", vertex.index, floattostring(vertex.x), floattostring(vertex.y), floattostring(vertex.z));
	}

	void AddTriangle(Triangle tri)
	{
		//Not bothering with cover right now, which is the last 4 values.
		_MESSAGE("NewArrayElement(nvnm, 'Triangles');seev(nvnm, 'Triangles\\[%i]', '%s %s %s %s %s %s 00 00 00 00');",
			tri.index, 
			uint32tostring(tri.vert1), 
			uint32tostring(tri.vert2),
			uint32tostring(tri.vert3),
			uint32tostring(tri.edge1),
			uint32tostring(tri.edge2),
			uint32tostring(tri.edge3));
	}

	// Remember to edit the navmesh slightly in the Creation Kit. Otherwise it seems to lead to an unending memory leak :D
	// I believe this is due to us not setting up the NavMeshGrid in the code, howeer making any change in the CK and saving it generates it.
	// Doing a balance for optimisation seems to work pretty well.
	void ExportNavmesh()
	{
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Skyrim Special Edition\\SKSE\\Navmesh.pas");
		_MESSAGE("unit userscript;uses SkyrimUtils;uses mteFunctions;");
		_MESSAGE("function NewArrayElement(rec: IInterface; path: String): IInterface; var a: IInterface; begin a := ElementByPath(rec, path); if Assigned(a) then begin Result := ElementAssign(a, HighInteger, nil, false); end else begin a := Add(rec, path, true);Result := ElementByIndex(a, 0);end;end;");
		_MESSAGE("function Process(e: IInterface): integer; var cell: IInterface; navm: IInterface; nvnm: IInterface; verts: IInterface; begin Result := 0;  if not (Signature(e) = 'CELL') then begin Exit; end; AddMessage('Processing: ' + FullPath(e));");
		_MESSAGE("navm := Add(e,'NAVM',true);nvnm := Add(navm,'NVNM',true);seev(nvnm, 'Version', 12);seev(nvnm, 'Parent Cell', HexFormID(e));seev(nvnm, 'NavMeshGrid', '00');");


		int VertCount = 0;
		int TriCount = 0;

		int QuadSize = 64;
		int corriderHeight = 6;

		VertList createdVerts = VertList();
		TriangleList createdTriangles = TriangleList();

		for (int z = 0; z < currentMap.size; z++)
		{
			for (int x = 0; x < currentMap.size; x++)
			{
				for (int y = 0; y < currentMap.size; y++)
				{
//					currentMap.map[x][y][z];
					Vert Vert1 = Vert(0, (x * (QuadSize * 2)) - QuadSize, (y * (QuadSize * 2)) - QuadSize, (z * (QuadSize * corriderHeight)));
					UInt32 Vert1Index = -1;
					Vert1Index = createdVerts.Find(Vert1);
					if (Vert1Index == -1)
					{
						Vert1.index = VertCount++;
						AddVertex(Vert1);
						Vert1Index = Vert1.index;
						createdVerts.AddItem(Vert1);
					}

					Vert Vert2 = Vert(0, (x * (QuadSize * 2)) + QuadSize, (y * (QuadSize * 2)) - QuadSize, (z * (QuadSize * corriderHeight)));
					UInt32 Vert2Index = -1;
					Vert2Index = createdVerts.Find(Vert2);
					if (Vert2Index == -1)
					{
						Vert2.index = VertCount++;
						AddVertex(Vert2);
						Vert2Index = Vert2.index;
						createdVerts.AddItem(Vert2);
					}

					Vert Vert3 = Vert(0, (x * (QuadSize * 2)) + QuadSize, (y * (QuadSize * 2)) + QuadSize, (z * (QuadSize * corriderHeight)));
					UInt32 Vert3Index = -1;
					Vert3Index = createdVerts.Find(Vert3);
					if (Vert3Index == -1)
					{
						Vert3.index = VertCount++;
						AddVertex(Vert3);
						Vert3Index = Vert3.index;
						createdVerts.AddItem(Vert3);
					}

					Vert Vert4 = Vert(0, (x * (QuadSize * 2)) - QuadSize, (y * (QuadSize * 2)) + QuadSize, (z * (QuadSize * corriderHeight)));
					UInt32 Vert4Index = -1;
					Vert4Index = createdVerts.Find(Vert4);
					if (Vert4Index == -1)
					{
						Vert4.index = VertCount++;
						AddVertex(Vert4);
						Vert4Index = Vert4.index;
						createdVerts.AddItem(Vert4);
					}

					Triangle tri1 = Triangle(TriCount++, Vert1Index, Vert2Index, Vert3Index, -1, -1, -1);
					createdTriangles.AddItem(tri1);

					Triangle tri2 = Triangle(TriCount++, Vert1Index, Vert4Index, Vert3Index, -1, -1, -1);
					createdTriangles.AddItem(tri2);

				}
			}
		}
		//Join the triangles
		for (int i = 0; i < createdTriangles.length; i++)
		{
			createdTriangles.data[i].edge1 = createdTriangles.FindNeighbours(createdTriangles.data[i], 1);
			createdTriangles.data[i].edge2 = createdTriangles.FindNeighbours(createdTriangles.data[i], 2);
			createdTriangles.data[i].edge3 = createdTriangles.FindNeighbours(createdTriangles.data[i], 3);
		}

		//Save the triangles
		for (int i = 0; i < createdTriangles.length; i++)
		{
			AddTriangle(createdTriangles.data[i]);
		}

		_MESSAGE("end;end.");
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Skyrim Special Edition\\SKSE\\Undaunted.log");
	}
	VertList* VertList::AddItem(Vert item)
	{
		VertList* currentlist = this;
		VertList newlist = VertList();
		newlist.length = currentlist->length + 1;
		newlist.data = new Vert[newlist.length];
		for (int i = 0; i < currentlist->length; i++)
		{
			newlist.data[i] = currentlist->data[i];
		}
		newlist.data[currentlist->length] = item;
		currentlist->data = newlist.data;
		currentlist->length = newlist.length;
		return currentlist;
	}

	UInt32 VertList::Find(Vert item)
	{
		VertList* currentlist = this;
		for (int i = 0; i < currentlist->length; i++)
		{
			if (currentlist->data[i].x == item.x &&
				currentlist->data[i].y == item.y &&
				currentlist->data[i].z == item.z)
			{
				return currentlist->data[i].index;
			}
		}
		return -1;
	}
	TriangleList* TriangleList::AddItem(Triangle item)
	{
		TriangleList* currentlist = this;
		TriangleList newlist = TriangleList();
		newlist.length = currentlist->length + 1;
		newlist.data = new Triangle[newlist.length];
		for (int i = 0; i < currentlist->length; i++)
		{
			newlist.data[i] = currentlist->data[i];
		}
		newlist.data[currentlist->length] = item;
		currentlist->data = newlist.data;
		currentlist->length = newlist.length;
		return currentlist;
	}

	/*
		Edge 1	uint16	Index within this list of the triangle that neighbors the first edge (vertex 0 to vertex 1)
		Edge 2	uint16	Index within this list of the triangle that neighbors the second edge (vertex 1 to vertex 2)
		Edge 3	uint16	Index within this list of the triangle that neighbors the third edge (vertex 2 to vertex 1)
	*/
	UInt32 TriangleList::FindNeighbours(Triangle item, int edge)
	{
		TriangleList* currentlist = this;
		if (edge == 1 || edge == 2)
		{
			for (int i = 0; i < currentlist->length; i++)
			{
				//Don't join with yourself.
				if (currentlist->data[i].index != item.index)
				{
					//South / West?
					if (currentlist->data[i].vert1 == item.vert2 && 
						currentlist->data[i].vert2 == item.vert3)
					{
						return currentlist->data[i].index;
					}
				}
			}
		}
		if (edge == 3)
		{
			for (int i = 0; i < currentlist->length; i++)
			{
				//Don't join with yourself.
				if (currentlist->data[i].index != item.index)
				{
					//Inner
					if (currentlist->data[i].vert1 == item.vert1 &&
						currentlist->data[i].vert3 == item.vert3)
					{
						return currentlist->data[i].index;
					}
				}
			}
		}
		return -1;
	}
}