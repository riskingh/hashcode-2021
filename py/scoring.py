import argparse
import bisect
from collections import deque as Queue
import tqdm


def _parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('input')
    parser.add_argument('output')
    return parser.parse_args()


args = _parse_args()


class Street(object):
    def __init__(self, name, length, begin, end, idx):
        self.name = name
        self.length = length
        self.begin = begin
        self.end = end
        self.idx = idx
        self.queue = Queue()
    
    def push(self, v_idx):
        self.queue.append(v_idx)

    def pop(self):
        return self.queue.popleft()

    def empty(self):
        return not bool(self.queue)


class Intersection(object):
    def __init__(self):
        self.input = set()
        self.output = set()
        self.schedule = None
        self.period = None
        self.schedule_prefix = None

    def add_in_street(self, idx):
        assert idx not in self.input
        self.input.add(idx)
    
    def add_out_street(self, idx):
        assert idx not in self.output
        self.output.add(idx)

    def set_schedule(self, schedule):
        self.schedule = schedule
        self.period = sum([t for _, t in self.schedule])
        self.schedule_prefix_sum = [0]
        for _, t in self.schedule:
            self.schedule_prefix_sum.append(self.schedule_prefix_sum[-1] + t)


    def get_current_street(self, timestamp):
        if self.schedule is None:
            return None

        if self.period == 0:
            return None
        reminder = timestamp % self.period
        schedule_idx = bisect.bisect(self.schedule_prefix_sum, reminder) - 1
        
        return self.schedule[schedule_idx][0]



class Vehicle(object):
    def __init__(self, path):
        self.path = path
        self.street_rel_idx = 0

    def move(self):
        self.street_rel_idx += 1
    
    def get_current_street(self):
        assert 0 <= self.street_rel_idx < len(self.path)
        return self.path[self.street_rel_idx]

    def is_finished(self):
        return self.street_rel_idx == len(self.path) - 1


streets = []
intersections = []
vehicles = []

STREET_NAME_TO_IDX = {}

with open(args.input) as fp:
    finput = lambda: fp.readline().strip()
    total_duration, num_intersections, num_streets, num_vehicles, finish_bonus = map(int, finput().split())
    intersections = [Intersection() for _ in range(num_intersections)]

    for idx in range(num_streets):
        begin, end, name, length = finput().split()
        begin = int(begin)
        end = int(end)
        length = int(end)
        streets.append(Street(name=name, length=length, begin=begin, end=end, idx=idx))
        intersections[begin].add_out_street(idx)
        intersections[end].add_in_street(idx)
        STREET_NAME_TO_IDX[name] = idx


    for v_idx in range(num_vehicles):
        p_names = finput().split()
        p = int(p_names[0])
        names = p_names[1:]
        path = []
        assert len(names) == p
        for name in names:
            s_idx = STREET_NAME_TO_IDX[name]
            path.append(s_idx)
        vehicles.append(Vehicle(path))
        streets[path[0]].push(v_idx)


with open(args.output) as fp:
    finput = lambda: fp.readline().strip()
    a = int(finput())
    assert 0 <= a <= num_intersections
    for _ in range(a):
        i_idx = int(finput())
        assert 0 <= i_idx < len(intersections)
        intersection = intersections[i_idx]
        assert intersection.schedule is None
        num_incoming_streets = int(finput())
        assert 0 <= num_incoming_streets <= len(intersection.input)
        schedule = []
        for street_rel_idx in range(num_incoming_streets):
            name, t = finput().split()
            t = int(t)
            assert 1 <= t <= total_duration
            assert name in STREET_NAME_TO_IDX
            street_idx = STREET_NAME_TO_IDX[name]
            assert street_idx in intersection.input
            schedule.append((street_idx, t))
        intersection.set_schedule(schedule)


CAR_EVENTS = [[] for _ in range(total_duration + 1)]

total_score = 0

for timestamp in tqdm.trange(0, total_duration + 1):
    print("TIMESTAMP", timestamp)
    # lookup all cat events and push them
    for event in CAR_EVENTS[timestamp]:
        car_idx, street_idx = event
        if vehicles[car_idx].is_finished():
            print('TIMESTAMP', timestamp, 'CAR', car_idx, 'IS FINISHED', 'SCORE', finish_bonus + (total_duration - timestamp))
            total_score += finish_bonus + (total_duration - timestamp)
        else:
            streets[street_idx].push(car_idx)
            print(vehicles[car_idx].path)
            print('TIMESTAMP', timestamp, 'car', car_idx, 'pushed', street_idx, streets[street_idx].name)

    for intersection_idx, intersection in enumerate(intersections):
        street_idx = intersection.get_current_street(timestamp)
        if street_idx is None:
            continue
        # print(intersection_idx, street_idx, streets[street_idx].name)
        if not streets[street_idx].empty():
            car_idx = streets[street_idx].pop()
            vehicles[car_idx].move()
            # assert not vehicles[car_idx].is_finished()
            next_street_idx = vehicles[car_idx].get_current_street()
            length = streets[next_street_idx].length
            if timestamp + length <= total_duration:
                CAR_EVENTS[timestamp + length].append((car_idx, next_street_idx))
                print('TIMESTAMP', timestamp, 'car', car_idx, 'next street', next_street_idx, streets[next_street_idx].name, length)
            else:
                print('Too long', timestamp + length, car_idx)

print(total_score)

