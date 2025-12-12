namespace rogue_unlike {

void prepare_hallucination() {
  set<string> names_seen;
  vector<entity*> visions;
  for(auto& [c, r]: rooms) for(auto& e: r.entities) {
    if(!e->can_be_hallucinated()) continue;
    string s = e->get_name();
    if(names_seen.count(s)) continue;
    names_seen.insert(s);
    visions.push_back(&*e);
    }

  for(auto& [c, r]: rooms) for(auto& e: r.entities) {
    e->hallucinated = hrand_elt(visions);
    }
  }

entity *entity::hal() {
  return (m.current.hallucinating && hallucinated) ? hallucinated : this;
  }

}
