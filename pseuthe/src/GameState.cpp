/*********************************************************************
Matt Marchant 2015
http://trederia.blogspot.com

pseuthe Zlib license.

This software is provided 'as-is', without any express or
implied warranty. In no event will the authors be held
liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute
it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented;
you must not claim that you wrote the original software.
If you use this software in a product, an acknowledgment
in the product documentation would be appreciated but
is not required.

2. Altered source versions must be plainly marked as such,
and must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any
source distribution.
*********************************************************************/

#include <GameState.hpp>
#include <CircleDrawable.hpp>
#include <GradientDrawable.hpp>
#include <Particles.hpp>
#include <App.hpp>
#include <Log.hpp>
#include <Util.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

namespace
{
    const int nubbinCount = 18;
}

GameState::GameState(StateStack& stateStack, Context context)
    : State     (stateStack, context),
    m_messageBus(),
    m_scene     (m_messageBus),
    m_physWorld (m_messageBus)
{
    context.renderWindow.setView(context.defaultView);
    
    for (int i = 0; i < 12; ++i)
        m_scene.addEntity(createEntity(sf::Color::White), Scene::Layer::FrontFront);

    for (int i = 0; i < 8; ++i)
        m_scene.addEntity(createEntity(sf::Color(170u, 170u, 170u)), Scene::Layer::FrontMiddle);

    for (int i = 0; i < 4; ++i)
        m_scene.addEntity(createEntity(sf::Color(85u, 85u, 85u)), Scene::Layer::FrontRear);

    //m_scene.addEntity(createEntity(m_messageBus, m_physWorld, sf::Color::White), Scene::Layer::FrontFront);

    m_scene.getLayer(Scene::Layer::BackRear).addComponent<GradientDrawable>(std::make_unique<GradientDrawable>(m_messageBus));
}

bool GameState::update(float dt)
{    
    while (!m_messageBus.empty())
    {
        Message msg = m_messageBus.poll();
        m_audioManager.handleMessage(msg);
        m_physWorld.handleMessages(msg);
        m_scene.handleMessages(msg);
    }

    m_audioManager.update(dt);
    m_physWorld.update(dt);
    m_scene.update(dt);

    return true;
}

void GameState::draw()
{
    getContext().renderWindow.draw(m_scene);
}

bool GameState::handleEvent(const sf::Event& evt)
{

    return true;
}

//private
Entity::Ptr GameState::createEntity(const sf::Color& colour)
{
    float size = static_cast<float>(Util::Random::value(10, 50));

    Entity::Ptr e = std::make_unique<Entity>(m_messageBus);
    CircleDrawable::Ptr cd = std::make_unique<CircleDrawable>(size, m_messageBus);
    cd->setColour(colour);
    e->addComponent<CircleDrawable>(cd);

    PhysicsComponent::Ptr pc = m_physWorld.addBody(size);
    e->addComponent<PhysicsComponent>(pc); 

    ParticleSystem::Ptr ps = ParticleSystem::create(Particle::Type::Echo, m_messageBus);
    ps->setTexture(getContext().appInstance.getTexture("assets/images/particles/circle.png"));
    auto particleSize = size * 2.f;
    ps->setParticleSize({ particleSize, particleSize });
    ps->setColour(colour);
    e->addComponent<ParticleSystem>(ps);


    ps = ParticleSystem::create(Particle::Type::Trail, m_messageBus);
    ps->setTexture(getContext().appInstance.getTexture("assets/images/particles/circle.png"));

    Entity::Ptr f = std::make_unique<Entity>(m_messageBus);
    f->addComponent<ParticleSystem>(ps);
    e->addChild(f);






    return std::move(e);
}