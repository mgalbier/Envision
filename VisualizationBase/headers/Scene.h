/***********************************************************************************************************************
 * Scene.h
 *
 *  Created on: Dec 6, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef SCENE_H_
#define SCENE_H_

#include "visualizationbase_api.h"
#include "ModelRenderer.h"

#include <QtGui/QGraphicsScene>

namespace Visualization {

class VISUALIZATIONBASE_API Scene : public QGraphicsScene
{
	private:
		ModelRenderer* renderer_;

	public:
		Scene();
		void setRenderer(ModelRenderer* renderer);

		ModelRenderer* renderer();
};

inline void Scene::setRenderer(ModelRenderer* renderer) { renderer_ = renderer; }
inline ModelRenderer* Scene::renderer() { return renderer_; }

}

#endif /* SCENE_H_ */
