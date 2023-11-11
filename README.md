# MiniProjects_and_Algos

A collection of miniature projects I've done in my freetime. 

___
## Human and Nature Dynamics (HANDY) Differential Equation Simulation

Based on the paper by Safa Motesharrei, Jorge Rivas, and Eugenia Kalnay: "Human and nature dynamics (HANDY): Modeling inequality and use of resources in the collapse or sustainability of societies," pursued off the recommendation of Dr. Carey King.

Consists of 4 differential equations...
1. Human Populations, divided into an "Elite" and "Commoner" population
2. Natural Supply, has a natural regeneration rate and depletion rate by humans
3. Wealth Supply, stock extracted from nature to be used by Human Populations and a buffer between drainage of natural supply & consumption by humans

![The Four Differential Equations we use as reference](content/4DifferentialEquations.png)

In our code they're represented by population structs:

```cpp
struct population { float stock, flow; } // x , dx/dt

// represent our other populations, have both a 'stock' and a 'flow' among other fields
struct human_pop : population;
struct nature_stock : population;
struct wealth_stock : population;

// initializer, there are actually four of these, one for each differential equation in the system
void init_population(... args);

human_pop& calcFlow(human_pop& hp);
nature_stock& calcFlow(nature_stock& nst, human_pop& commoner_hp);
wealth_stock& calcFlow(wealth_stock& wst, nature_stock& nst, human_pop& elite_hp, human_pop& commoner_hp);
```

Calculating the solution is a matter of just taking the integral of the calculated flow. This is further simplified since our System of Differential Equations is Autonomous.
```cpp
population& calcStock(population& pop, const float begin, const float end, const bool use_prev) {
	const float dt = 0.001f;
	float sum = 0.0f + pop.stock * use_prev;
	float t = begin;

	while (t < end) {
		sum += pop.flow * dt;
		t += dt;
	}

	bool eq_bounds = begin == end;
	pop.stock = sum * !eq_bounds + pop.stock * eq_bounds;
	return pop;
}
```

Here are a couple example graphs I got by taking the values I got via the simulation, transferring them into a .csv, and plotting them using matplotlib:

![Graph of an Oscillatory Equilibrium simulation run](output/NoEliteEquillibrium)

Extra note: the reason for the struct reference returns is because method chaining is cool that's pretty much it lol

Citation:

Safa Motesharrei, Jorge Rivas, Eugenia Kalnay, 
Human and nature dynamics (HANDY): Modeling inequality and use of resources in the collapse or sustainability of societies, 
Ecological Economics, 
Volume 101, 
2014, 
Pages 90-102, 
ISSN 0921-8009, 
https://doi.org/10.1016/j.ecolecon.2014.02.014. (https://www.sciencedirect.com/science/article/pii/S0921800914000615)

Abstract: There are widespread concerns that current trends in resource-use are unsustainable, but possibilities of overshoot/collapse remain controversial. Collapses have occurred frequently in history, often followed by centuries of economic, intellectual, and population decline. Many different natural and social phenomena have been invoked to explain specific collapses, but a general explanation remains elusive. In this paper, we build a human population dynamics model by adding accumulated wealth and economic inequality to a predator–prey model of humans and nature. The model structure, and simulated scenarios that offer significant implications, are explained. Four equations describe the evolution of Elites, Commoners, Nature, and Wealth. The model shows Economic Stratification or Ecological Strain can independently lead to collapse, in agreement with the historical record. The measure “Carrying Capacity” is developed and its estimation is shown to be a practical means for early detection of a collapse. Mechanisms leading to two types of collapses are discussed. The new dynamics of this model can also reproduce the irreversible collapses found in history. Collapse can be avoided, and population can reach a steady state at maximum carrying capacity if the rate of depletion of nature is reduced to a sustainable level and if resources are distributed equitably.

Keywords: Human–nature dynamics; Societal collapse; Carrying capacity; Overshoot vs. sustainability; Economic inequality; Ecological strain
___
## JJK Domains

The gist of this project is modelling a construct called "Domains" from a show called ![Jujutsu Kaisen](https://en.wikipedia.org/wiki/Jujutsu_Kaisen). To focus squarely on domains; they're spherical constructs created by a special kind of energy generated and manipulated by characaters in the series called Cursed Energy. A barrier has 3 main components to be recognized as complete: **a barrier**, an associated **innate domain**, and an **imbued technique**. 

#### Todo List

- [ ] **GENERAL:**
    - [x] Render loop
    - [x] Mouse click & position detection
    - [ ] Background 
        - [x] General background
        - [ ] Dark mode toggle?
        - [ ] Some sort of map overlay would be cool
        - [ ] Numbers on grid as reference
    - [ ] Project title somewhere on screen
    - [ ] View matrix 
        - [ ] More dynamic camera or zooming functionality
        - [ ] Fix fullscreen stuffs
- [ ] **DOMAIN CLASS:**
    - [x] General domain parent class
        - [ ] Tug of wars
            - _make tug of wars better later_
            - _use actual physics as reference_
        - [x] Make domain transparent on tug of wars
        - [x] General getter functions
            - GetRadius
            - GetBoundingBox
            - Get Refinement
            - ...
        - [ ] Set upper bound on domain size during tug of wars
            - Have refinement be inversely proportional to domain size?
    - [ ] Finish Closed Domains
        - [ ] Turn of detection on WorldEntities w/out CE
    - [ ] Finish Open Domains
        - [x] Get OpenDomain.shader working for segmented animation
        - [ ] Maintain detection of WorldEntities w/out CE
    - [ ] Finish Simple Domains
        - [ ] Remove tug of war capability
    - [x] Working collision detection
- [ ] **WORD ENTITIES:**
    - [ ] CE Objects
        - [ ] Sorcerers (can deploy domains)
        - [ ] Non-sorcerers (cannot deploy domains)
    - [ ] Structures
- [ ] **DATA STRUCTURES & MANAGERS:**
    - [ ] Domain Manager
        - [x] Domain Manager working with std::vector
            - Replace w/ Quadtree or BVH Tree
        - [x] Domain Insertion
            - [x] `bool add(Domain& d)`
            - [x] `bool add(Domain* d)`
            - [x] `bool add(DomainType type, float radius = 150.f, sf::Color color = sf::Color::Red, float refine_val = 0.5f, sf::Vector2f centerCoords = sf::Vector2f(0.f, 0.f));`
        - [x] Collision detection working
        - [ ] Add a toggle-able text list for debugging
    - [ ] UI Manager
        - [ ] Finish all the UI elements first lol
    - [ ] Quadtree
        - [ ] Append to domain manager
    - [ ] BVH Tree
        - [ ] Append to domain manager
- [ ] **UI ELEMENTS:**
    - [ ] Button class
        - [x] Get clicks working
        - [x] Callback
        - [ ] Finish shader for rounding the button's corners
        - [ ] Domain Buttons
            - [x] Make domain spawning button
            - [x] Make button for changing domain type (OPEN, CLOSED, SIMPLE)
    - [ ] Slider class
        - [x] Callback 
        - [ ] Draggable slider
            - [ ] Way to set upper and lower bounds
        - [ ] Domain Sliders
            - [ ] Slider to change domain radius
            - [ ] Slider to change domain refinement
    - [ ] UIGroup (TBD)
        - [ ] Popup UI Groups
            - [ ] Make them appear on right click
            - [ ] Couple UIGroup w/ Domains & World Entities
        - [ ] General UI Group
        - [ ] Setup shared callback struct among groups & sub-UI Elements

#### Domains
###### OpenDomains
![Red Open Domain](content/redopendomain_scrs.png)
###### ClosedDomains
![Blue Closed Domain](content/bluecloseddomain_scrs.png)