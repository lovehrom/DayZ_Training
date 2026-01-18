# AI Behavior Trees in DayZ - Complete Guide

**Status:** ✅ Advanced Guide
**Difficulty:** Advanced
**Time to Learn:** 75 minutes
**Related:** [AI System](../reference/AI_SYSTEM.md) | [NPC Classes](../reference/AI_NPC_CLASSES.md)

---

## ⚡ 30-Second Summary

Behavior Trees are a hierarchical system for making AI decisions. Instead of simple "if-else", AI uses a tree structure of nodes to determine actions, enabling complex and realistic AI behavior.

---

## 🎯 What You'll Learn

1. **Behavior Tree Basics** - Nodes, trees, execution
2. **Node Types** - Sequence, Selector, Decorator
3. **Creating Custom Behaviors** - Zombie/NPC AI
4. **Real-World Examples** - Patrol, Attack, Flee
5. **Performance Optimization** - Efficient AI

---

## 💡 What are Behavior Trees?

### Concept

**Behavior Tree** = Tree of decision nodes

**Simple AI (If-Else):**
```c
void AIUpdate()
{
    if (CanSeeEnemy())
        Attack();
    else if (CanHearEnemy())
        Investigate();
    else
        Patrol();
}
```

**Behavior Tree:**
```
Root
├─ Sequence (Attack)
│  ├─ CanSeeEnemy?
│  └─ Attack
├─ Selector (Investigate or Patrol)
│  ├─ Sequence (Investigate)
│  │  ├─ CanHearEnemy?
│  │  └─ Investigate
│  └─ Patrol
```

### Benefits

✅ **Modular** - Easy to add/remove behaviors
✅ **Reusable** - Nodes can be reused
✅ **Debuggable** - Easy to see what AI is doing
✅ **Extensible** - Easy to add complex behaviors

---

## 🔧 Node Types

### 1. Sequence Node

**Execute children in order. Fail if ANY child fails.**

```
Sequence: "Open Door"
├─ Walk to Door
├─ Unlock Door
└─ Open Door
```

**Code:**
```c
class SequenceNode: BehaviorNode
{
    protected ref array<ref BehaviorNode> m_Children;

    override bool Execute(AIAgent agent)
    {
        foreach (BehaviorNode child : m_Children)
        {
            if (!child.Execute(agent))
                return false;  // Fail fast
        }
        return true;  // All children succeeded
    }
}
```

**When to use:**
- ✅ Actions that MUST happen in order
- ✅ All steps must succeed

---

### 2. Selector Node

**Execute children in order. Succeed if ANY child succeeds.**

```
Selector: "Get Food"
├─ Eat in Inventory
├─ Hunt Animal
└─ Find Berries
```

**Code:**
```c
class SelectorNode: BehaviorNode
{
    protected ref array<ref BehaviorNode> m_Children;

    override bool Execute(AIAgent agent)
    {
        foreach (BehaviorNode child : m_Children)
        {
            if (child.Execute(agent))
                return true;  // Success
        }
        return false;  // All children failed
    }
}
```

**When to use:**
- ✅ Try multiple options
- ✅ First success wins

---

### 3. Decorator Node

**Modifies child behavior (repeat, invert, timeout, etc.)**

**Repeat (3 times):**
```c
class RepeatNode: BehaviorNode
{
    protected BehaviorNode m_Child;
    protected int m_Repeats;

    override bool Execute(AIAgent agent)
    {
        for (int i = 0; i < m_Repeats; i++)
        {
            m_Child.Execute(agent);
        }
        return true;
    }
}
```

**Invert (NOT):**
```c
class InverterNode: BehaviorNode
{
    protected BehaviorNode m_Child;

    override bool Execute(AIAgent agent)
    {
        return !m_Child.Execute(agent);
    }
}
```

---

## 🧟 Creating Custom Behaviors

### Example 1: Zombie AI

**Behavior:**
1. If see enemy → Attack
2. If hear enemy → Chase
3. Else → Wander

```c
class ZombieBehaviorTree
{
    protected BehaviorNode m_Root;

    void ZombieBehaviorTree()
    {
        // Build tree
        SelectorNode selector = new SelectorNode();

        // Option 1: Attack
        SequenceNode attackSeq = new SequenceNode();
        attackSeq.AddChild(new CanSeeEnemyNode());
        attackSeq.AddChild(new AttackNode());
        selector.AddChild(attackSeq);

        // Option 2: Chase
        SequenceNode chaseSeq = new SequenceNode();
        chaseSeq.AddChild(new CanHearEnemyNode());
        chaseSeq.AddChild(new ChaseNode());
        selector.AddChild(chaseSeq);

        // Option 3: Wander
        selector.AddChild(new WanderNode());

        m_Root = selector;
    }

    void Update(ZombieBase zombie)
    {
        m_Root.Execute(zombie);
    }
}

// Leaf nodes
class CanSeeEnemyNode: BehaviorNode
{
    override bool Execute(AIAgent agent)
    {
        ZombieBase zombie = ZombieBase.Cast(agent);
        return zombie.CanSeeEnemy();
    }
}

class AttackNode: BehaviorNode
{
    override bool Execute(AIAgent agent)
    {
        ZombieBase zombie = ZombieBase.Cast(agent);
        zombie.Attack();
        return true;
    }
}

class CanHearEnemyNode: BehaviorNode
{
    override bool Execute(AIAgent agent)
    {
        ZombieBase zombie = ZombieBase.Cast(agent);
        return zombie.CanHearEnemy();
    }
}

class ChaseNode: BehaviorNode
{
    override bool Execute(AIAgent agent)
    {
        ZombieBase zombie = ZombieBase.Cast(agent);
        zombie.ChaseEnemy();
        return true;
    }
}

class WanderNode: BehaviorNode
{
    override bool Execute(AIAgent agent)
    {
        ZombieBase zombie = ZombieBase.Cast(agent);
        zombie.Wander();
        return true;
    }
}
```

---

### Example 2: Guard NPC

**Behavior:**
1. If see intruder → Raise alarm
2. If intruder in zone → Attack
3. If see suspicious → Investigate
4. Else → Patrol

```c
class GuardBehaviorTree
{
    protected BehaviorNode m_Root;

    void GuardBehaviorTree()
    {
        SelectorNode selector = new SelectorNode();

        // Priority 1: Raise alarm
        SequenceNode alarmSeq = new SequenceNode();
        alarmSeq.AddChild(new IsIntruderNode());
        alarmSeq.AddChild(new IsIntruderArmedNode());
        alarmSeq.AddChild(new RaiseAlarmNode());
        selector.AddChild(alarmSeq);

        // Priority 2: Attack
        SequenceNode attackSeq = new SequenceNode();
        attackSeq.AddChild(new IsIntruderInZoneNode());
        attackSeq.AddChild(new AttackNode());
        selector.AddChild(attackSeq);

        // Priority 3: Investigate
        SequenceNode investigateSeq = new SequenceNode();
        investigateSeq.AddChild(new IsSuspiciousNode());
        investigateSeq.AddChild(new InvestigateNode());
        selector.AddChild(investigateSeq);

        // Priority 4: Patrol
        selector.AddChild(new PatrolNode());

        m_Root = selector;
    }
}
```

---

## 📊 Real-World Examples

### Patrol Behavior

**Patrol between waypoints:**

```c
class PatrolNode: BehaviorNode
{
    protected array<vector> m_Waypoints;
    protected int m_CurrentWaypoint;

    override bool Execute(AIAgent agent)
    {
        Man ai = Man.Cast(agent);

        if (m_CurrentWaypoint >= m_Waypoints.Count())
            m_CurrentWaypoint = 0;

        vector target = m_Waypoints[m_CurrentWaypoint];
        ai.MoveToPos(target);

        if (vector.Distance(ai.GetPosition(), target) < 2)
        {
            m_CurrentWaypoint++;
        }

        return true;
    }
}
```

---

### Flee Behavior

**Run away from enemy:**

```c
class FleeNode: BehaviorNode
{
    override bool Execute(AIAgent agent)
    {
        Man ai = Man.Cast(agent);
        Man enemy = ai.GetEnemy();

        if (!enemy)
            return false;

        // Get direction away from enemy
        vector dir = ai.GetPosition() - enemy.GetPosition();
        dir.Normalize();

        // Run 100 meters away
        vector fleePos = ai.GetPosition() + dir * 100;
        ai.MoveToPos(fleePos);

        return true;
    }
}
```

---

### Group Behavior

**Multiple NPCs working together:**

```c
class GroupAttackNode: BehaviorNode
{
    override bool Execute(AIAgent agent)
    {
        Man leader = Man.Cast(agent);
        array<Man> group = GetGroupMembers(agent);

        // Attack as group
        foreach (Man member : group)
        {
            member.Attack leader.GetEnemy();
        }

        return true;
    }
}
```

---

## ⚡ Performance Optimization

### Don't Update Every Frame

**❌ BAD:**
```c
override void EOnFrame(IEntity other, float timeSlice)
{
    m_BehaviorTree.Execute(this);  // Every frame = LAG
}
```

**✅ GOOD:**
```c
protected float m_LastUpdateTime;

override void EOnFrame(IEntity other, float timeSlice)
{
    float now = GetGame().GetTickTime();

    // Update every 0.5 seconds
    if (now - m_LastUpdateTime > 0.5)
    {
        m_BehaviorTree.Execute(this);
        m_LastUpdateTime = now;
    }
}
```

---

### Use Cooldowns

```c
class CooldownNode: BehaviorNode
{
    protected BehaviorNode m_Child;
    protected float m_Cooldown;
    protected float m_LastExecute;

    override bool Execute(AIAgent agent)
    {
        float now = GetGame().GetTickTime();

        if (now - m_LastExecute < m_Cooldown)
            return false;

        m_LastExecute = now;
        return m_Child.Execute(agent);
    }
}
```

---

### Cache Results

```c
class HasTargetNode: BehaviorNode
{
    protected bool m_HasTarget;
    protected float m_LastCheck;

    override bool Execute(AIAgent agent)
    {
        float now = GetGame().GetTickTime();

        // Cache for 1 second
        if (now - m_LastCheck < 1.0)
            return m_HasTarget;

        m_LastCheck = now;
        m_HasTarget = (agent.GetTarget() != null);
        return m_HasTarget;
    }
}
```

---

## 📝 Quick Reference

### Node Types

| Node Type | Behavior | Use Case |
|-----------|----------|----------|
| **Sequence** | Execute all, fail if any fails | Steps that must all succeed |
| **Selector** | Execute until success | Try multiple options |
| **Decorator** | Modify child behavior | Repeat, Invert, Timeout |

### Common Behaviors

| Behavior | Pattern | Complexity |
|----------|---------|------------|
| Patrol | Sequence of waypoints | Easy |
| Attack | CanSeeEnemy → Attack | Easy |
| Chase | CanHearEnemy → Chase | Easy |
| Flee | Run away from enemy | Medium |
| Group | Coordinate with allies | Hard |

---

## ⚠️ Common Mistakes

### ❌ Mistake 1: Deep Trees

**Problem:** Too many nested nodes

```c
// WRONG: 20+ levels deep
Root → Seq → Seq → Seq → Seq → ... → Action
```

**Solution:** Keep tree shallow (< 5 levels)

### ❌ Mistake 2: Expensive Checks

```c
// WRONG: Raycast every frame
class CanSeeEnemyNode: BehaviorNode
{
    override bool Execute(AIAgent agent)
    {
        // Expensive raycast!
        return agent.CanSeeEnemy();  // Raycast
    }
}

// CORRECT: Cache result
class CanSeeEnemyNode: BehaviorNode
{
    override bool Execute(AIAgent agent)
    {
        // Check every 0.5 seconds
        if (GetGame().GetTickTime() - m_LastCheck < 0.5)
            return m_CachedResult;

        m_CachedResult = agent.CanSeeEnemy();
        return m_CachedResult;
    }
}
```

---

## 🎓 Next Steps

**After learning behavior trees:**
1. **Read:** [AI NPC Classes](../reference/AI_NPC_CLASSES.md) - NPC system
2. **Example:** Create custom zombie behaviors
3. **Example:** Create guard NPCs with patrols

---

## 💡 Key Takeaways

1. **Behavior Trees = Decision Making**
   - Nodes = Decisions/actions
   - Tree = Hierarchy of decisions
   - Execution = Top-to-bottom

2. **Node Types:**
   - Sequence = All must succeed
   - Selector = First success wins
   - Decorator = Modify behavior

3. **Common Patterns:**
   - Attack (CanSeeEnemy → Attack)
   - Patrol (Waypoints)
   - Flee (Run away)
   - Group (Coordinate)

4. **Performance:**
   - Don't update every frame
   - Use cooldowns
   - Cache expensive checks

---

**Last Updated:** 2025-01-18
**Status:** ✅ Complete
