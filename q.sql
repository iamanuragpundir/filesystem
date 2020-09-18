-- 1. Find the names of all females who eat both mushroom and pepperoni pizza.
/*

select p.name from Person as p natural join Eats e where e.name  = p.name and e.pizza = 'pepperoni' and p.gender = 'female'
intersect
select p.name from Person as p natural join Eats e where e.name  = p.name and e.pizza = 'mushroom' and p.gender = 'female' ;

select p.name from Person p cross join Eats e
where e.name  = p.name and e.pizza = 'pepperoni' and p.name in
(select p.name from Person as p natural join Eats e 
where e.name  = p.name and e.pizza = 'mushroom' and p.gender = 'female') ;


select p.name from Person p, Eats e where p.name = e.name and (e.pizza = 'mushroom' or e.pizza = 'pepperoni') 
intersect
select p.name from Person p, Eats e where p.name = e.name and e.pizza = 'pepperoni' and p.gender = 'female'
intersect
select p.name from Person p, Eats e where p.name = e.name and e.pizza = 'mushroom' ;

*/



-- 2. Find all pizzerias that are frequented by only females or only males.
/*
select distinct f.pizzeria from Person p, Frequents f where f.name = p.name and p.gender = 'male'
and f.pizzeria not in (select distinct f.pizzeria from Person p, Frequents f where f.name = p.name and p.gender = 'female')
union
select distinct f.pizzeria from Person p, Frequents f where f.name = p.name and p.gender = 'female'
and f.pizzeria not in (select distinct f.pizzeria from Person p, Frequents f where f.name = p.name and p.gender = 'male') ;

(select f.pizzeria from Person p, Frequents f where f.name = p.name and p.gender = 'male'
EXCEPT
select f.pizzeria from Person p, Frequents f where f.name = p.name and p.gender = 'female')
union
(select  f.pizzeria from Person p, Frequents f where f.name = p.name and p.gender = 'female'
EXCEPT select f.pizzeria from Person p, Frequents f where f.name = p.name and p.gender = 'male') ;

select distinct f.pizzeria from Person p, Frequents f where p.name  = f.name 
EXCEPT
(select f.pizzeria from Person p, Frequents f where p.name  = f.name and p.gender = 'male' 
intersect
select f.pizzeria from Person p, Frequents f where p.name  = f.name and p.gender = 'female'  );
*/

-- 3. Find all pizzerias that serve at least one pizza that Amy eats for less than $10.00.
/*
select distinct s.pizzeria from Serves s, Eats e where e.pizza = s.pizza and s.price < 10 and e.name  = 'Amy';

select  distinct s.pizzeria from Eats e, Person p, Serves s where e.name = p.name and e.pizza = s.pizza and p.name = 'Amy'
and s.price < 10 ;

select distinct pizzeria from Serves where price < 10 and pizza in
(select e.pizza from Eats e, Person p where e.name = p.name and p.name = 'Amy') ;
*/

--4. For each person, find all pizzas the person eats that are not served by any pizzeria the person frequents.
/*
select  e.name, e.pizza from Eats e where pizza not in
(select s.pizza from Frequents f, Serves s where s.pizzeria = f.pizzeria and f.name = e.name) ;

select p.name, e.pizza from Person p, Eats e where e.name = p.name 
EXCEPT
select f.name, s.pizza from Serves s, Frequents f where s.pizzeria = f.pizzeria ;

select e.name, e.pizza from Serves s, Eats e where e.pizza = s.pizza
EXCEPT
select f.name, s.pizza from Serves s, Frequents f where s.pizzeria = f.pizzeria ;
*/

-- 5. 
/*
select pizzeria from Serves where price in
(select min(price) from Serves where pizza = 'pepperoni' );

select distinct pizzeria from Serves where pizzeria not in
(select pizzeria from Serves where pizzeria not in
(select pizzeria from Serves where price in
(select min(price) from Serves where pizza = 'pepperoni' )));
*/















