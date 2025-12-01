--1
with ac as (
	select i.name i_name, a.name a_name, count(aa.aid) aa_count
	from z_institution i
	join z_article_institution ai on i.iid = ai.iid
	join z_article a on ai.aid = a.aid
	join z_article_author aa on aa.aid = a.aid
	where i.town = 'Brno'
	group by i.name, a.name
),

mc as (
	select ac.i_name, max(ac.aa_count) max_aa
	from ac
	group by ac.i_name
)

select ac.i_name, ac.a_name, mc.max_aa
from ac, mc
where ac.i_name = mc.i_name and ac.aa_count	= mc.max_aa
order by ac.i_name, ac.a_name

--2
with jc1 as (
	select j.name, count(i.name) journal_count
	from z_institution i
	join z_article_institution ai on i.iid = ai.iid
	join z_article a on ai.aid = a.aid
	join z_journal j on a.jid = j.jid
	join z_year_field_journal yfj on a.jid = yfj.jid and a.year = yfj.year
	where yfj.year = 2018 and i.name = 'Vysoká škola báòská - Technická univerzita Ostrava'
	group by j.name
),

jc2 as (
	select j.name, count(i.name) journal_count
	from z_institution i
	join z_article_institution ai on i.iid = ai.iid
	join z_article a on ai.aid = a.aid
	join z_journal j on a.jid = j.jid
	join z_year_field_journal yfj on a.jid = yfj.jid and a.year = yfj.year
	where yfj.year = 2018 and i.name = 'Vysoké uèení technické v Brnì'
	group by j.name
)

select jc1.name
from jc1, jc2
where jc1.name = jc2.name and jc1.journal_count > jc2.journal_count
order by jc1.name

--3

with counts as (
	select aa.aid, count(aa.rid) aa_count
	from z_article_author aa
	join z_article a on a.aid = aa.aid
	where a.year = 2018
	group by aa.aid
)

select a.rid, a.name
from z_author a
join z_article_author aa on a.rid = aa.rid
join z_article ar on aa.aid = ar.aid
join counts c on ar.aid = c.aid
where a.name like 'falt%' and ar.year = 2018
group by a.rid, a.name
having min(c.aa_count) >= 100
order by a.name;

--1
with ac as (
	select ff.name ff_name, a.name a_name, count(aa.aid) author_count
	from z_field_ford ff
	join z_field_of_science fos on ff.sid = fos.sid
	join z_year_field_journal yfj on ff.fid = yfj.fid
	join z_article a on a.jid = yfj.jid and yfj.year = a.year
	join z_article_author aa on a.aid = aa.aid
	where yfj.ranking = 'Decil' and fos.name = 'Social Sciences'
	and yfj.year in (2019, 2020)
	group by ff.name, a.name
),

ma as (
	select ac.ff_name, max(ac.author_count) max_author
	from ac
	group by ac.ff_name
)

select ma.ff_name, ac.a_name, ma.max_author
from ac, ma
where ma.ff_name = ac.ff_name and ma.max_author = ac.author_count
order by ac.ff_name, ac.a_name;

--2
with pubs2019 as (
	select ff.fid, ff.name, count(distinct a.aid) publikace
	from z_field_ford ff
	join z_field_of_science fos on ff.sid = fos.sid
	join z_year_field_journal yfj on ff.fid = yfj.fid
	join z_article a on a.jid = yfj.jid and yfj.year = a.year
	where yfj.ranking = 'Q1' and fos.name = 'Social Sciences'
	and yfj.year = 2019
	group by ff.fid, ff.name
),

pubs2020 as (
	select ff.fid, ff.name, count(distinct a.aid) publikace
	from z_field_ford ff
	join z_field_of_science fos on ff.sid = fos.sid
	join z_year_field_journal yfj on ff.fid = yfj.fid
	join z_article a on a.jid = yfj.jid and yfj.year = a.year
	where yfj.ranking = 'Q1' and fos.name = 'Social Sciences'
	and yfj.year = 2020
	group by ff.fid, ff.name
)

select pubs2020.fid, pubs2020.name,
coalesce(pubs2019.publikace, 0), pubs2020.publikace,
pubs2020.publikace - coalesce(pubs2019.publikace, 0) rozdil
from pubs2020
left join pubs2019 on pubs2019.fid = pubs2020.fid;

--3
with autori as (
	select distinct ff.fid
	from z_field_ford ff
	join z_year_field_journal yfj on ff.fid = yfj.fid
	join z_article ar on yfj.jid = ar.jid and yfj.year = ar.year
	join z_article_author aa on ar.aid = aa.aid
	where yfj.year = 2019
	group by ff.fid, aa.aid
	having count(aa.aid) > 10
)

select ff.name
from z_field_ford ff
where ff.fid not in (select fid from autori);

--1
with ac as (
	select ff.name, a.aid, count(aa.aid) author_count
	from z_field_ford ff
	join z_field_of_science fos on ff.sid = fos.sid
	join z_year_field_journal yfj on ff.fid = yfj.fid
	join z_article a on yfj.jid = a.jid and yfj.year = a.year
	join z_article_institution ai on a.aid = ai.aid
	join z_institution i on ai.iid = i.iid
	join z_article_author aa on a.aid = aa.aid
	where fos.name = 'Natural sciences' and yfj.ranking = 'Q1'
	and i.name = 'Fakultní nemocnice Ostrava'
	group by ff.name, a.aid
)

select ac.name, avg(ac.author_count + 0.0) prumery
from ac
group by ac.name;

--2
with instituce1 as (
	select i.iid 
	from z_institution i
	join z_article_institution ai on i.iid = ai.iid
	join z_article a on ai.aid = a.aid
	join z_year_field_journal yfj on a.jid = yfj.jid and a.year = yfj.year
	where yfj.ranking = 'Q1' and i.town like '%Brno%' and a.year = 2020
),
instituce2 as (
	select i.iid 
	from z_institution i
	join z_article_institution ai on i.iid = ai.iid
	join z_article a on ai.aid = a.aid
	join z_year_field_journal yfj on a.jid = yfj.jid and a.year = yfj.year
	where yfj.ranking = 'Q2' and i.town like '%Brno%' and a.year = 2020
),
instituce3 as (
	select i.iid 
	from z_institution i
	join z_article_institution ai on i.iid = ai.iid
	join z_article a on ai.aid = a.aid
	join z_year_field_journal yfj on a.jid = yfj.jid and a.year = yfj.year
	where yfj.ranking = 'Q3' and i.town like '%Brno%' and a.year = 2020
),
instituce4 as (
	select i.iid 
	from z_institution i
	join z_article_institution ai on i.iid = ai.iid
	join z_article a on ai.aid = a.aid
	join z_year_field_journal yfj on a.jid = yfj.jid and a.year = yfj.year
	where yfj.ranking = 'Q4' and i.town like '%Brno%' and a.year = 2020
)

select i.iid, i.name, count(a.aid) author_count
from z_institution i
join z_article_institution ai on i.iid = ai.iid
join z_article a on ai.aid = a.aid
where a.year = 2020
and i.iid in (select iid from instituce1)
and i.iid in (select iid from instituce2)
and i.iid in (select iid from instituce3)
and i.iid in (select iid from instituce4)
group by i.iid, i.name

--3
select au.name
from z_author au
join z_article_author aa on au.rid = aa.rid
join z_article a on a.aid = aa.aid
join z_article_institution ai on a.aid = ai.aid
join z_institution i on ai.iid = i.iid
where i.name like '%bio%'
group by au.name
having count(distinct i.name) >= 5