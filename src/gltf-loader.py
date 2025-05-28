import struct
import base64
import json
import sys

def component_datatype(cmpnnt_typename):
    cmpnnt_type={5126:"float",5123:"ushort"}
    cmpnnt_type_bytes={"float":4,"ushort":2}
    numbytes=cmpnnt_type_bytes[cmpnnt_type[cmpnnt_typename]]
    return numbytes

def type_size(typename):
    datasize={"VEC3":3,"VEC2":2,"SCALAR":1}
    numbytes=datasize[typename]
    return numbytes

def load_gltf_file():
    if(len(sys.argv)!=2):
        print("Usage: python3 gltf-loader.py filepath")
        sys.exit()
    gltf_filename=sys.argv[1]
    assert gltf_filename.endswith(".gltf"),"not a .gltf file"

    with open(gltf_filename) as gltffile:
        contents=json.load(gltffile)
    return (contents,gltf_filename)

def fetch_uri_data(contents):
    buffers=contents["buffers"]
    buffer=buffers[0]
    data=buffer["uri"].split(",")[1]
    b64_data=data
    binary=base64.b64decode(b64_data)
    return binary

def fetch_vertices(contents, endianness):
    meshes=contents["meshes"]
    accessors=contents["accessors"]
    buffer_views=contents["bufferViews"]

    pos_index=meshes[0]["primitives"][0]["attributes"]["POSITION"]
    vertices_count=accessors[pos_index]["count"]
    vert_datatype=component_datatype(accessors[pos_index]["componentType"])
    vert_size=type_size(accessors[pos_index]["type"])
    buffview=accessors[pos_index]["bufferView"]
    vert_buffviews=buffer_views[buffview]
    vert_offset_bytes=vert_buffviews["byteOffset"]
    vert_length_bytes=vert_buffviews["byteLength"]

    index=0
    size=vert_datatype*vert_size
    print(f"Vertices number: {vertices_count}. Getting vertices...")
    vertices=[]
    while index<vertices_count: 
        start=vert_offset_bytes+index*size
        vertex_bytes=binary[start:start+size]
        vertex = struct.unpack(endianness+"3f",vertex_bytes)
        vertices.append(vertex)
        index+=1
    return vertices

def fetch_indices(contents, endianness):
    meshes=contents["meshes"]
    accessors=contents["accessors"]
    buffer_views=contents["bufferViews"]

    ind_index=meshes[0]["primitives"][0]["indices"]
    indices_count=accessors[ind_index]["count"]
    index_datatype=component_datatype(accessors[ind_index]["componentType"])
    index_size=type_size(accessors[ind_index]["type"])
    buffview=accessors[ind_index]["bufferView"]
    ind_buffviews=buffer_views[buffview]
    ind_offset_bytes=ind_buffviews["byteOffset"]
    ind_length_bytes=ind_buffviews["byteLength"]

    index=0
    size=index_datatype*index_size
    print(f"Indices number: {indices_count}. Getting indices...")
    indices=[]
    while index<indices_count:
        start=ind_offset_bytes+index*size
        index_bytes=binary[start:start+size]
        ind, = struct.unpack(endianness+"H",index_bytes)
        indices.append(ind)
        index+=1
    return indices

contents,filename=load_gltf_file()
binary=fetch_uri_data(contents)
endianness=sys.byteorder
endianness="<" if endianness=="little" else">"
vertices=fetch_vertices(contents, endianness)
indices=fetch_indices(contents, endianness)
print(vertices)
print(indices)

data_file=f"../assets/{filename[:-5].split('/')[-1]}.txt"
print(data_file)
with open(data_file,"w") as f:
    f.write(f"VERTICES {len(vertices)}\n")
    for vertex in vertices:
        f.write(f"{vertex[0]} {vertex[1]} {vertex[2]}\n")
    f.write(f"INDICES {len(indices)}\n")
    for index in indices:
        f.write(f"{index}\n")
