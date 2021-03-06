//================================================//

#include "Sparks.hpp"

//================================================//

namespace Sparks
{

//================================================//

void translateDotSceneNode(Ogre::SceneNode* node, Ogre::Vector3 vec)
{
	Ogre::SceneNode::ChildNodeIterator itr = node->getChildIterator();
	for(;itr.hasMoreElements();){
		Ogre::SceneNode* child = static_cast<Ogre::SceneNode*>(itr.getNext());
		child->translate(vec);
	}
}

//================================================//

void scaleDotSceneNode(Ogre::SceneNode* node, Ogre::Vector3 vec)
{
	Ogre::SceneNode::ChildNodeIterator itr = node->getChildIterator();
	for(;itr.hasMoreElements();){
		Ogre::SceneNode* child = static_cast<Ogre::SceneNode*>(itr.getNext());
		child->scale(vec);
	}
}

//================================================//

void rotateDotSceneNode(Ogre::SceneNode* node, Ogre::Quaternion q)
{
	Ogre::SceneNode::ChildNodeIterator itr = node->getChildIterator();
	for(;itr.hasMoreElements();){
		Ogre::SceneNode* child = static_cast<Ogre::SceneNode*>(itr.getNext());
		child->rotate(q);
	}
}

//================================================//

void setDotSceneNodePosition(Ogre::SceneNode* node, Ogre::Vector3 vec)
{
	Ogre::SceneNode::ChildNodeIterator itr = node->getChildIterator();
	for(;itr.hasMoreElements();){
		Ogre::SceneNode* child = static_cast<Ogre::SceneNode*>(itr.getNext());
		child->setPosition(vec);
	}
}

//================================================//

void destroyAllAttachedMovableObjects(Ogre::SceneNode* sceneNode)
{
	if(!sceneNode){
		return;
	}

	Ogre::SceneNode::ObjectIterator itr = sceneNode->getAttachedObjectIterator();

	for(;itr.hasMoreElements();){
		Ogre::MovableObject* object = static_cast<Ogre::MovableObject*>(itr.getNext());
		sceneNode->getCreator()->destroyMovableObject(object);
	}

	Ogre::SceneNode::ChildNodeIterator itrChild = sceneNode->getChildIterator();

	for(;itrChild.hasMoreElements();){
		Ogre::SceneNode* child = static_cast<Ogre::SceneNode*>(itrChild.getNext());
		destroyAllAttachedMovableObjects(child);
	}
}

//================================================//

Ogre::Vector3 Sparks::getWorldPosition(Ogre::SceneNode* node)
{
	Ogre::Vector3 pos = node->_getDerivedPosition();

	//pos = node->_getDerivedPosition() + node->_getDerivedOrientation() * node->_getDerivedScale() * node->getParentSceneNode()->_getDerivedPosition();

	Ogre::SceneNode* parent = node->getParentSceneNode();
	while(parent != NULL){
		parent = parent->getParentSceneNode();
	}

	pos = parent->convertLocalToWorldPosition(pos);

	return pos;
}

//================================================//

Ogre::Vector3 Sparks::bulletToOgreVector3(btVector3& vec)
{
	return Ogre::Vector3(vec.getX(), vec.getY(), vec.getZ());
}

//================================================//

btVector3 Sparks::ogreToBulletVector3(Ogre::Vector3& vec)
{
	return btVector3(vec.x, vec.y, vec.z);
}

//================================================//

void Sparks::GetMeshInformation(const Ogre::MeshPtr mesh,
                                size_t &vertex_count,
                                Ogre::Vector3* &vertices,
                                size_t &index_count,
                                Ogre::uint32* &indices,
                                const Ogre::Vector3 &position,
                                const Ogre::Quaternion &orient,
                                const Ogre::Vector3 &scale)
{
	bool added_shared = false;
    size_t current_offset = 0;
    size_t shared_offset = 0;
    size_t next_offset = 0;
    size_t index_offset = 0;

    vertex_count = index_count = 0;

    // Calculate how many vertices and indices we're going to need
    for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
    {
        Ogre::SubMesh* submesh = mesh->getSubMesh( i );

        // We only need to add the shared vertices once
        if(submesh->useSharedVertices)
        {
            if( !added_shared )
            {
                vertex_count += mesh->sharedVertexData->vertexCount;
                added_shared = true;
            }
        }
        else
        {
            vertex_count += submesh->vertexData->vertexCount;
        }

        // Add the indices
        index_count += submesh->indexData->indexCount;
    }


    // Allocate space for the vertices and indices
    vertices = new Ogre::Vector3[vertex_count];
    indices = new Ogre::uint32[index_count];

    added_shared = false;

    // Run through the submeshes again, adding the data into the arrays
    for ( unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
    {
        Ogre::SubMesh* submesh = mesh->getSubMesh(i);

        Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;

        if((!submesh->useSharedVertices)||(submesh->useSharedVertices && !added_shared))
        {
            if(submesh->useSharedVertices)
            {
                added_shared = true;
                shared_offset = current_offset;
            }

            const Ogre::VertexElement* posElem =
                vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);

            Ogre::HardwareVertexBufferSharedPtr vbuf =
                vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());

            unsigned char* vertex =
                static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

            // There is _no_ baseVertexPointerToElement() which takes an Ogre::Ogre::Real or a double
            //  as second argument. So make it float, to avoid trouble when Ogre::Ogre::Real will
            //  be comiled/typedefed as double:
            //      Ogre::Ogre::Real* pOgre::Real;
            float* pReal;

            for( size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
            {
                posElem->baseVertexPointerToElement(vertex, &pReal);

                Ogre::Vector3 pt(pReal[0], pReal[1], pReal[2]);

                vertices[current_offset + j] = (orient * (pt * scale)) + position;
            }

            vbuf->unlock();
            next_offset += vertex_data->vertexCount;
        }


        Ogre::IndexData* index_data = submesh->indexData;
        size_t numTris = index_data->indexCount / 3;
        Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;

        bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);

        Ogre::uint32*  pLong = static_cast<Ogre::uint32*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
        unsigned short* pShort = reinterpret_cast<unsigned short*>(pLong);


        size_t offset = (submesh->useSharedVertices)? shared_offset : current_offset;

        if ( use32bitindexes )
        {
            for ( size_t k = 0; k < numTris*3; ++k)
            {
                indices[index_offset++] = pLong[k] + static_cast<Ogre::uint32>(offset);
            }
        }
        else
        {
            for ( size_t k = 0; k < numTris*3; ++k)
            {
                indices[index_offset++] = static_cast<Ogre::uint32>(pShort[k]) +
                    static_cast<Ogre::uint32>(offset);
            }
        }

        ibuf->unlock();
        current_offset = next_offset;
    }
}

//================================================//

} // namespace Sparks

//================================================//